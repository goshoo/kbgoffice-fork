#include "application.h"
//#include <QtGui/QApplication>

#include <QtGui/QSystemTrayIcon>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QTranslator>
#include <QTextCodec>
#include <QTime>
#include <QSettings>
#include <stdio.h>
#include <unistd.h>
#include "encodechar.h"
#include "config.h"

#include "mainwindow.h"

#ifdef DATA_DIR_CONFIG
    static const char DATA_DIR[] = DATA_DIR_CONFIG;
#else
    #ifdef Q_OS_WIN32
        static const char DATA_DIR[] = ".";
    #else
        static const char DATA_DIR[] = "/usr/local/share/bgoffice/";
    #endif
#endif

QTextCodec* Converter1251;

static const char OPTION_INTERFACE_LANGUAGE[]       = "InterfaceLanguage"; // same as in mainwindow
static const char MISSING_DATA[] = QT_TRANSLATE_NOOP("Application",
        "<html>Cannot find dictionary files.\n"
        "Please look at README for additional information and help or "
        "visit <a style='text-decoration:none;' "
        "href='http://bgoffice.sourceforge.net'>http://bgoffice.sourceforge.net</a> "
        "to download dictionaries.</html>");
static const char ALREADY_RUNNING[] = QT_TRANSLATE_NOOP("Application", "Another copy already running.");
static const char NO_SYSTRAY[] = QT_TRANSLATE_NOOP("Application", "I couldn't detect any system tray on this system.");

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(kbgoffice);
    QCoreApplication::setOrganizationName("bgoffice");
    QCoreApplication::setOrganizationDomain("bgoffice.sourceforge.net");
    QCoreApplication::setApplicationName("kbgoffice"); // BG Office Assistant

//#ifdef Q_OS_WIN32
//    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName());
#ifdef Q_WS_WIN
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif
    QSettings settings;
//    QSettings settings( fileName, QSettings::defaultFormat()) // old style: getenv("HOME")+'/'+".kbgoffice"
//    /home/user/.config/bgoffice/kbgoffice.conf // BG Office Assistant.conf
//    C:\Documents and Settings\user\Application Data\bgoffice\kbgoffice.ini // BG Office Assistant.ini

//  dataDir search order is: kbgoffice.conf, built-in, executable's dir="."
    QStringList sl;

    QString dataDir = settings.value("DataDir", "").toString();
    if( !dataDir.isEmpty() )
        sl << dataDir;
    if( !QString(DATA_DIR).isEmpty() )
        sl << DATA_DIR;
    sl << ".";

    sl.removeDuplicates(); // ".", "/path"
    int i = sl.indexOf(".");
    while(i != -1) {
        QFileInfo fi(QDir::fromNativeSeparators(argv[0]));
        sl[i] = fi.absolutePath();
        i = sl.indexOf(".");
    }
    sl.removeDuplicates(); // "." might expand to duplicate path

    dataDir.clear();
    for(i=0; i<sl.size(); i++){
        QDir dir(sl.at(i), "*.dict");
        if( !dir.entryList().isEmpty() ){
            dataDir = sl[i];
            break;
        }
    }

    if(!dataDir.endsWith('/'))
        dataDir += '/';
    sl.clear();

/*  Win32: No console if GUI so:
    0. kbgoffice --dump-data-dir | more # Works always
    1. CONFIG += console and FreeConsole() when done - yet console shows for split second
    2. MessageBox()
#ifdef Q_OS_WIN32
#include <windows.h>
#endif
        static const char DUMP_DATA_DIR[] = "kbgoffice --dump-data-dir";
        #ifdef Q_OS_WIN32
        MessageBox(NULL, dataDir.toStdWString().data(), QString(DUMP_DATA_DIR).toStdWString().data(), MB_OK);
        #endif
    3. AttachConsole(-1) doesn't always work! Supported in XP SP1+ */

    if ( (argc > 1) && (QString::compare(argv[1], "--dump-data-dir", Qt::CaseInsensitive) == 0) ) {
        printf("%s\n", QDir::toNativeSeparators(DATA_DIR).toLocal8Bit().constData());
        return 0;
    }

    Converter1251 = QTextCodec::codecForName("CP1251");

    QTranslator qtr;
    QString sLang = settings.value(OPTION_INTERFACE_LANGUAGE, QLocale::system().name()).toString();
    if(!qtr.load("kbgoffice_" + sLang, dataDir)){
        sLang = QLocale::system().name();
        if(!qtr.load("kbgoffice_" + sLang, dataDir)){
            sLang = "en";
            qtr.load("kbgoffice_" + sLang, dataDir);
        }
    }

    Application a(argc, argv);
    a.installTranslator(&qtr);

    QSharedMemory sm(QCoreApplication::applicationName());
    if( !sm.create(1, QSharedMemory::ReadOnly) ){
        //printf("%s\n", ALREADY_RUNNING);
        QMessageBox::critical(0, QCoreApplication::applicationName(), a.tr(ALREADY_RUNNING));
        return 2;
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QCoreApplication::applicationName(), a.tr(NO_SYSTRAY));
        //return 1;
    }
    Application::setQuitOnLastWindowClosed(false);

// If the system's clock changed since the last re-start() the result is undefined!
// e.g. when daylight savings time is turned on or off.
    QTime t;
    bool useSplashScreen = settings.value("UseSplashScreen", true).toBool();
    QSplashScreen splash(QPixmap(), Qt::WindowStaysOnTopHint);
    if (useSplashScreen) {
        t.start();
        QPixmap pixmap(":/images/kbgoffice_splash.png");
        splash.setPixmap(pixmap);
        splash.setFixedSize(pixmap.size()); // Fix4 Special App/Window settings resizing/tiling image
        splash.show();
        a.processEvents();
    }

    MainWindow w(&settings, dataDir);

    // Translation file for application strings
    w.qtTranslator.load("qt_" + sLang, dataDir);
    a.installTranslator(&w.qtTranslator);
    w.appTranslator.load("kbgoffice_" + sLang, dataDir);
    a.installTranslator(&w.appTranslator);
    a.removeTranslator(&qtr);

    splash.activateWindow(); // isVisible() won't work while closing on mouse click below

    // Check data
    if (!w.isDataOk()) {
        if (useSplashScreen)
            splash.close();

        // Center app, otherwise error win appears in center of app
        w.move(QApplication::desktop()->screenGeometry().center() - w.rect().center());

        QMessageBox::critical(0, w.windowTitle(), a.tr(MISSING_DATA));

        return 1;
    }

    if (useSplashScreen) {
        // Wait couple of seconds before hiding splash screen
        do {
            usleep(200);
            a.processEvents();
        } while (splash.isVisible() && (t.elapsed() < 2000));
        splash.finish(&w);
    }

    w.show();

    return a.exec();
}
