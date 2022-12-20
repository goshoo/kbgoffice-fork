#include "mainwindow.h"
#include "testdialog.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "ui_info.h"
#include "ui_options.h"
#include "ui_testdialog.h"

#include <QClipboard>
#include <QDir>
#include <QFontDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProcess>
#include <QStyleFactory>
#include <QTemporaryFile>
#include <QTimer>
#include <QWhatsThis>

#include "config.h"
#include "encodechar.h"

// Some Constants
static const int  MAX_WORD_LEN           = 70;
static const int  MAX_WORDS_ALLOWED      = 7;
static const int  SPACE_IN_DIALOGS       = 10;
static const char CAPTION[]              = QT_TRANSLATE_NOOP("MainWindow", "BG Office Assistant");
static const char FOUND[]                = QT_TRANSLATE_NOOP("MainWindow", ":-)");
static const char NOT_FOUND[]            = QT_TRANSLATE_NOOP("MainWindow", ":-(");
static const char SIMILAR[]              = QT_TRANSLATE_NOOP("MainWindow", ":-|");
static const char MISSING_ABRR_FILE[]    = QT_TRANSLATE_NOOP("MainWindow", "<FONT color=\"red\">Error: Abbreviations file is missing.</FONT>");

//static const char TITLE_ACTION_TEXT[]         = CAPTION[];
static const char WATCH_CLIPBOARD_ACTION_TEXT[] = QT_TRANSLATE_NOOP("MainWindow", "&Watch Clipboard");
static const char MINIMIZE_ACTION_TEXT[]        = QT_TRANSLATE_NOOP("MainWindow", "Mi&nimize");
static const char MAXIMIZE_ACTION_TEXT[]        = QT_TRANSLATE_NOOP("MainWindow", "Ma&ximize");
static const char RESTORE_ACTION_TEXT[]         = QT_TRANSLATE_NOOP("MainWindow", "&Restore");
static const char QUIT_ACTION_TEXT[]            = QT_TRANSLATE_NOOP("MainWindow", "&Quit");

static const char COLOR_FOUND[]          = "background-color: #00FF00"; // green
static const char COLOR_NOT_FOUND[]      = "background-color: #FF0000"; // red
static const char COLOR_SIMILAR[]        = "background-color: #FFFF00"; // yellow

static const char SEARCH_DICTIONARY[]   = "http://www.infoplease.com/search.php3?in=all&query=";
static const char SEARCH_ENCYCLOPEDIA[] = "http://www.infoplease.com/search.php3?in=encyclopedia&query=";
static const char SEARCH_ALL[]          = "http://www.infoplease.com/search.php3?in=encyclopedia&query=";

static const char CREATION_TEMP_FILE_FAILED[] = QT_TRANSLATE_NOOP("MainWindow",
        "Failed to create a temporaty file.\n"
        "Check your write permissions in \"%1\" directory.");

/*: This is a comment for the translator!
    Please translate 'English' as language translating to. e.g. Deutsch.
    Do not translate as English!    */
static const char INTERFACE_LANG[] = QT_TRANSLATE_NOOP("MainWindow", "English");

// Default options
static const int  DEFAULT_INPUT_FIELD_WIDTH        = 200;
static const int  DEFAULT_LEFT_PANEL_WIDTH         = 200;
static const int  DEFAULT_WORDS_IN_LIST            = 250;
static const char DEFAULT_WORDS_IN_HISTORY         = 50;
static const int  DEFAULT_BORDER_WIDTH             = 5;
static const bool DEFAULT_SHOW_TOOL_BAR            = false;
static const bool DEFAULT_DOCK_TOOL_BAR            = false;
static const bool DEFAULT_TRANSLATE_ON_EVERY_KEY   = true;
static const bool DEFAULT_WATCH_CLIPBOARD          = true;
static const bool DEFAULT_ADVANCED_CLIPBOARD       = false;
static const bool DEFAULT_ADVANCED_OPTIONS         = false;
static const bool DEFAULT_SEPARATE_MEANINGS        = true;
static const bool DEFAULT_TRY_WITHOUT_SUFFIX       = true;
static const bool DEFAULT_LATIN_OUTPUT             = false;
static const bool DEFAULT_LATIN_INPUT              = false;
static const char DEFAULT_FONT_NAME[]              = "*";
static const int  DEFAULT_FONT_SIZE                = 14;
static const int  DEFAULT_FONT_WEIGHT              = 50;
static const bool DEFAULT_FONT_ITALIC              = false;
static const char DEFAULT_GEOMETRY[]               = "40, 40, 600, 400";
static const char DEFAULT_SYSTEM_STYLE_NAME[]      = "Windows";
static const bool DEFAULT_ADV_SRCH_HIGHLIGHT       = true;
static const bool DEFAULT_ADV_SRCH_WHOLE_WORD      = true;
static const bool DEFAULT_ADV_SRCH_EXACT_PHRASE    = false;
static const bool DEFAULT_ADV_SRCH_ASD_IGNORE_KEY  = true;
static const bool DEFAULT_ADV_SRCH_ASD_IGNORE_HIS  = true;
static const bool DEFAULT_ADV_SRCH_ASD_ENTER       = true;
static const bool DEFAULT_ADV_SRCH_ASD_DO_NOT_REPL = true;
static const char DEFAULT_BROWSER_NAME[]           = "firefox";
static const bool DEFAULT_USE_EXTERNAL_BROWSER     = true;
static const bool DEFAULT_USE_SPLASH_SCREEN        = true;
static const bool DEFAULT_ICON_IN_SYSTRAY          = true;
static const bool DEFAULT_CLOSE_HIDE2_SYSTRAY      = true;
static const bool DEFAULT_MINIMIZE_TO_SYSTRAY      = false;

// Min & Max options
static const int  MAX_INPUT_FIELD_WIDTH = 500;
static const int  MIN_INPUT_FIELD_WIDTH = 25;
static const int  MAX_LEFT_PANEL_WIDTH  = 500;
static const int  MIN_LEFT_PANEL_WIDTH  = 0;
static const int  MAX_WORDS_IN_LIST     = 500;
static const int  MIN_WORDS_IN_LIST     = 0;
static const int  MAX_WORDS_IN_HISTORY  = 200;
static const int  MIN_WORDS_IN_HISTORY  = 0;
static const int  MAX_BORDER_WIDTH      = 25;
static const int  MIN_BORDER_WIDTH      = 0;

// Strings in registry
static const char OPTION_INPUT_FIELD_WIDTH[]        = "InputFieldWidth";
static const char OPTION_LEFT_PANEL_WIDTH[]         = "LeftPanelWidth";
static const char OPTION_WORDS_IN_LIST[]            = "WordsInList";
static const char OPTION_WORDS_IN_HISTORY[]         = "WordsInHistory";
static const char OPTION_BORDER_WIDTH[]             = "BorderWidth";
static const char OPTION_SHOW_TOOL_BAR[]            = "ShowToolBar";
static const char OPTION_DOCK_TOOL_BAR[]            = "DockToolBar";
static const char OPTION_SYSTEM_STYLE_NAME[]        = "SystemStyleName";
static const char OPTION_TRANSLATE_ON_EVERY_KEY[]   = "TraslateOnEveryKey";
static const char OPTION_WATCH_CLIPBOARD[]          = "WatchClipboard";
static const char OPTION_ADVANCED_CLIPBOARD[]       = "AdvancedClipboard";
static const char OPTION_ADVANCED_OPTIONS[]         = "AdvancedOptions";
static const char OPTION_SEPARATE_MEANINGS[]        = "SeparateMeanings";
static const char OPTION_TRY_WITHOUT_SUFFIX[]       = "TryWithoutSuffix";
static const char OPTION_FONT_NAME[]                = "FontName";
static const char OPTION_FONT_SIZE[]                = "FontSize";
static const char OPTION_FONT_WEIGHT[]              = "FontWeight";
static const char OPTION_FONT_ITALIC[]              = "FontItalic";
static const char OPTION_GEOMETRY[]                 = "Geometry";
static const char OPTION_LAST_USED_DICTIONARY[]     = "LastUsedDictionary";
static const char OPTION_ADV_SRCH_HIGHLIGHT[]       = "AdvSrch.Highlight";
static const char OPTION_ADV_SRCH_WHOLE_WORD[]      = "AdvSrch.WholeWord";
static const char OPTION_ADV_SRCH_EXACT_PHRASE[]    = "AdvSrch.EaxctPhrase";
static const char OPTION_ADV_SRCH_ASD_IGNORE_KEY[]  = "AdvSrch.IgnoreOnEveryKey";
static const char OPTION_ADV_SRCH_ASD_IGNORE_HIS[]  = "AdvSrch.IgnoreFromHistory";
static const char OPTION_ADV_SRCH_ASD_ENTER[]       = "AdvSrch.EnterSearch";
static const char OPTION_ADV_SRCH_ASD_DO_NOT_REPL[] = "AdvSrch.DoNotReplacePhrase";
static const char OPTION_DEFAULT_BROWSER[]          = "DefaultBrowser";
static const char OPTION_USE_EXTERNAL_BROWSER[]     = "UseExternalBrowser";
static const char OPTION_USE_SPLASH_SCREEN[]        = "UseSplashScreen";
static const char OPTION_INTERFACE_LANGUAGE[]       = "InterfaceLanguage";
static const char OPTION_ICON_IN_SYSTRAY[]          = "SysTrayIcon";
static const char OPTION_CLOSE_HIDE2_SYSTRAY[]      = "CloseHidesToSysTray";
static const char OPTION_MINIMIZE_TO_SYSTRAY[]      = "MinimizeToSysTray";

MainWindow::MainWindow(QSettings *pProp, QString sDataDir, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->word->installEventFilter(this);
    ui->list->installEventFilter(this);

    // Keep reference to some vars
    prop = pProp;
    strncpy(dataDir, sDataDir.toLatin1().data(), 1023);
    dataDir[1023] = '\0';

    // Create and Init Dictionary object
    dictionary = new TranslatorManager(dataDir);
    dataOk = dictionary->init();

    // Init some variables
    fromSystemHighlighted = false;
    fromListHighlighted = false;
    fromHistoryBuffer = false;
    fromListSelected = false;
    fromClipboard = false;
    fromSystem = false;
    fromTextChanged = false;
    fromAdvancedSearch = false;
    sysTrayIcon = true;
    closeToSysTray = true;
    minimizeToSysTray = false;
    strLink = "";

    // Read data from ini file and set it
    inputFieldWidth = prop->value(OPTION_INPUT_FIELD_WIDTH, DEFAULT_INPUT_FIELD_WIDTH).toInt();
    if(inputFieldWidth < MIN_INPUT_FIELD_WIDTH)
        inputFieldWidth = MIN_INPUT_FIELD_WIDTH;
    else if(inputFieldWidth > MAX_INPUT_FIELD_WIDTH)
        inputFieldWidth = MAX_INPUT_FIELD_WIDTH;

    leftPanelWidth = prop->value(OPTION_LEFT_PANEL_WIDTH, DEFAULT_LEFT_PANEL_WIDTH).toInt();
    if(leftPanelWidth < MIN_LEFT_PANEL_WIDTH)
        leftPanelWidth = MIN_LEFT_PANEL_WIDTH;
    else if(leftPanelWidth > MAX_LEFT_PANEL_WIDTH)
        leftPanelWidth = MAX_LEFT_PANEL_WIDTH;

    wordsInList = prop->value(OPTION_WORDS_IN_LIST, DEFAULT_WORDS_IN_LIST).toInt();
    if(wordsInList < MIN_WORDS_IN_LIST)
        wordsInList = MIN_WORDS_IN_LIST;
    else if(wordsInList > MAX_WORDS_IN_LIST)
        wordsInList = MAX_WORDS_IN_LIST;

    borderWidth = prop->value(OPTION_BORDER_WIDTH, DEFAULT_BORDER_WIDTH).toInt();
    if(borderWidth < MIN_BORDER_WIDTH)
        borderWidth = MIN_BORDER_WIDTH;
    else if(borderWidth > MAX_BORDER_WIDTH)
        borderWidth = MAX_BORDER_WIDTH;

    wordsInHistory = prop->value(OPTION_WORDS_IN_HISTORY, DEFAULT_WORDS_IN_HISTORY).toInt();
    if(wordsInHistory < MIN_WORDS_IN_HISTORY)
        wordsInHistory = MIN_WORDS_IN_HISTORY;
    else if(wordsInHistory > MAX_WORDS_IN_HISTORY)
        wordsInHistory = MAX_WORDS_IN_HISTORY;

    translateOnEveryKey = prop->value(OPTION_TRANSLATE_ON_EVERY_KEY, DEFAULT_TRANSLATE_ON_EVERY_KEY).toBool();
    watchClipboard = prop->value(OPTION_WATCH_CLIPBOARD, DEFAULT_WATCH_CLIPBOARD).toBool();
    advancedClipboard = prop->value(OPTION_ADVANCED_CLIPBOARD, DEFAULT_ADVANCED_CLIPBOARD).toBool();
    advancedOptions = prop->value(OPTION_ADVANCED_OPTIONS, DEFAULT_ADVANCED_OPTIONS).toBool();
    showToolBar = prop->value(OPTION_SHOW_TOOL_BAR, DEFAULT_SHOW_TOOL_BAR).toBool();
    dockToolBar = prop->value(OPTION_DOCK_TOOL_BAR, DEFAULT_DOCK_TOOL_BAR).toBool();
    advSrchIgnoreKey = prop->value(OPTION_ADV_SRCH_ASD_IGNORE_KEY, DEFAULT_ADV_SRCH_ASD_IGNORE_KEY).toBool();
    advSrchIgnoreHistory = prop->value(OPTION_ADV_SRCH_ASD_IGNORE_HIS, DEFAULT_ADV_SRCH_ASD_IGNORE_HIS).toBool();
    advSrchEnterSearch = prop->value(OPTION_ADV_SRCH_ASD_ENTER, DEFAULT_ADV_SRCH_ASD_ENTER).toBool();
    advSrchDoNotReplacePhrase = prop->value(OPTION_ADV_SRCH_ASD_DO_NOT_REPL, DEFAULT_ADV_SRCH_ASD_DO_NOT_REPL).toBool();
    if(!systemClipboard->supportsSelection())
        advancedClipboard = FALSE;

    browserName = prop->value(OPTION_DEFAULT_BROWSER, DEFAULT_BROWSER_NAME).toString();
    useExternalBrowser = prop->value(OPTION_USE_EXTERNAL_BROWSER, DEFAULT_USE_EXTERNAL_BROWSER).toBool();
    useSplashScreen = prop->value(OPTION_USE_SPLASH_SCREEN, DEFAULT_USE_SPLASH_SCREEN).toBool();

    sysTrayIcon = prop->value(OPTION_ICON_IN_SYSTRAY, DEFAULT_ICON_IN_SYSTRAY).toBool();
    closeToSysTray = prop->value(OPTION_CLOSE_HIDE2_SYSTRAY, DEFAULT_CLOSE_HIDE2_SYSTRAY).toBool();
    minimizeToSysTray = prop->value(OPTION_MINIMIZE_TO_SYSTRAY, DEFAULT_MINIMIZE_TO_SYSTRAY).toBool();
    if (QSystemTrayIcon::isSystemTrayAvailable()) {

        //Create Actions
        titleAction = new QAction(tr(CAPTION), this);
        titleAction->setIcon(windowIcon());

        watchClipboardAction = new QAction(tr(WATCH_CLIPBOARD_ACTION_TEXT), this);
        watchClipboardAction->setCheckable(true);
        watchClipboardAction->setIcon(QIcon(":/icons/klipper.png"));
        connect(watchClipboardAction, SIGNAL(triggered()), this, SLOT(slotWatchClipboardOptionChanged()));

        minimizeAction = new QAction(tr(MINIMIZE_ACTION_TEXT), this);
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

        maximizeAction = new QAction(tr(MAXIMIZE_ACTION_TEXT), this);
        connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

        restoreAction = new QAction(tr(RESTORE_ACTION_TEXT), this);
        connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

        quitAction = new QAction(tr(QUIT_ACTION_TEXT), this);
        quitAction->setIcon(QIcon(":/icons/exit.png"));
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

        ui->action_Configure->setIcon(QIcon(":/icons/configure.png"));
/*  // Good replacements :)
        /usr/share/icons/oxygen/16x16/apps/klipper.png
        /usr/share/icons/oxygen/16x16/actions/configure.png
        /usr/share/icons/hicolor/16x16/actions/exit.png

        watchClipboardAction->setIcon(QIcon::fromTheme("edit-paste", QIcon(":/icons/klipper.png")));
        minimizeAction->setIcon(QIcon::fromTheme("document-revert")); //go-bottom
        maximizeAction->setIcon(QIcon::fromTheme("view-fullscreen"));
        restoreAction->setIcon(QIcon::fromTheme("view-restore"));
        quitAction->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/exit.png")));
*/
        //createTrayIcon
        trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(titleAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(watchClipboardAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(minimizeAction);
        trayIconMenu->addAction(maximizeAction);
        trayIconMenu->addAction(restoreAction);
        trayIconMenu->addAction(ui->action_Configure);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);

        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->setToolTip(tr(CAPTION));
        trayIcon->setIcon(windowIcon());
        //Remember to setWindowIcon() if we change trayIcon and vv

        connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

        if(sysTrayIcon)
            trayIcon->show();
    }

    if (dataOk) {
            dictionary->setSeparateMeanings(prop->value(OPTION_SEPARATE_MEANINGS, DEFAULT_SEPARATE_MEANINGS).toBool());
            dictionary->setTryWithoutSuffix(prop->value(OPTION_TRY_WITHOUT_SUFFIX, DEFAULT_TRY_WITHOUT_SUFFIX).toBool());
            dictionary->setAdvancedSearchWholeWord(prop->value(OPTION_ADV_SRCH_WHOLE_WORD, DEFAULT_ADV_SRCH_WHOLE_WORD).toBool());
            dictionary->setAdvancedSearchExactPhrase(prop->value(OPTION_ADV_SRCH_EXACT_PHRASE, DEFAULT_ADV_SRCH_EXACT_PHRASE).toBool());
            dictionary->setAdvancedSearchHighlight(prop->value(OPTION_ADV_SRCH_HIGHLIGHT, DEFAULT_ADV_SRCH_HIGHLIGHT).toBool());
            dictionary->setBoldDecoration(true);
            dictionary->setHTMLOutput(true);
            dictionary->setCurrentDictionary(prop->value(OPTION_LAST_USED_DICTIONARY, dictionary->getCurrentDictionary()).toInt());
    }

    // Create history object
    history = new History(wordsInHistory, MAX_WORD_LEN);

    // Set system style if not KDE
    systemStyleName = prop->value(OPTION_SYSTEM_STYLE_NAME, DEFAULT_SYSTEM_STYLE_NAME).toString(); //W2U() .toLocal8Bit().data()

    // Set font, read from file settings and set it
    font = QFont(
            //W2U(prop->value(OPTION_FONT_NAME, DEFAULT_FONT_NAME).toString().toLocal8Bit().data()),
            prop->value(OPTION_FONT_NAME, DEFAULT_FONT_NAME).toString(),
            prop->value(OPTION_FONT_SIZE, DEFAULT_FONT_SIZE).toInt(),
            prop->value(OPTION_FONT_WEIGHT, DEFAULT_FONT_WEIGHT).toInt(),
            prop->value(OPTION_FONT_ITALIC, DEFAULT_FONT_ITALIC).toBool());

    // Set caption of window
    setWindowTitle(tr(CAPTION));
    // Set icon of window
    QPixmap p1(":/images/kbgoffice_icon_mini.png");
    setWindowIcon(p1);

    // Set positions and size of window
    int X = 0, Y = 0, W = 0, H = 0;
    sscanf(DEFAULT_GEOMETRY, "%d, %d, %d, %d", &X, &Y, &W, &H);
    sscanf(prop->value(OPTION_GEOMETRY, DEFAULT_GEOMETRY).toString().toLocal8Bit().data(), "%d, %d, %d, %d", &X, &Y, &W, &H);
    resize(QSize(W, H));
    move(QPoint(X, Y));

    ui->action_Quit->setIcon(QIcon(":/icons/exit.png"));

    if (dataOk) {
        QActionGroup *dicts = new QActionGroup(ui->menu_Dictionaries);
        dicts->setExclusive(true);
        for (int i = 0; i < dictionary->sizeOfDictionaries(); i++) {
            Dictionary d = dictionary->getDictionary(i);
            QAction *act = new QAction(QString("&%1. " + W2U(d.name.c_str())).arg(i + 1), dicts);
            act->setData(i);
            act->setCheckable(true);
            act->setChecked(dictionary->getCurrentDictionary() == i);
            act->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1+i));
            act->setStatusTip(tr("Activates %1").arg(W2U(d.name.c_str())));
            act->setIcon(QIcon(QPixmap(d.icon.c_str())));
            //act->setText(W2U(d.name.c_str()));
            act->setToolTip(W2U(d.name.c_str()));
            connect(act, SIGNAL(triggered()), this, SLOT(menu_Dict()));
            if(dictionary->getCurrentDictionary() == i)
                ui->action_Info->setIcon(act->icon());
        }
        ui->menu_Dictionaries->insertActions(ui->menu_Dictionaries->actions().at(0), dicts->actions());
        ui->mainToolBar->addActions(dicts->actions());
    }

    //ui->mainToolBar->setFixedHeight(ui->bclear->height());
    if (showToolBar) {
        if(dockToolBar) {
                ui->upperLayout->insertWidget(14, ui->mainToolBar, 0, Qt::AlignCenter);
            } else {
                this->addToolBar(ui->mainToolBar);
            }
        ui->mainToolBar->show();
    } else {
        ui->mainToolBar->hide();
    }

    ui->action_Show_ToolBar->setChecked(showToolBar);
    if (!advancedOptions)
        ui->menuBar->removeAction(ui->menu_Translation->menuAction());
        // setVisible(false); wont work (QAction prevents)

    ui->word->setMaxLength(MAX_WORD_LEN);
    if (!advancedOptions) {
        ui->menu_Edit->removeAction(ui->actionSearch_in_Dic_tionary);
        ui->menu_Edit->removeAction(ui->actionSearch_in_Encyclopedia);
        ui->menu_Edit->removeAction(ui->actionSearch_in_A_ll);
        ui->bsearchDict->hide();
        ui->bsearchEncycl->hide();
        ui->bsearchAll->hide();
    }

    // Set size, positions and font of widgets
    setView(systemStyleName);
    ui->word->setFocus();

    systemClipboard = QApplication::clipboard();
    connect(systemClipboard, SIGNAL(dataChanged()), this, SLOT(slotClipboardChanged()));

    if(systemClipboard->supportsSelection())
        connect(systemClipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(slotClipboardChanged(QClipboard::Mode)));
//    if(systemClipboard->supportsSelection())
//        connect(systemClipboard, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
// Signal selectionChanged() won't be emited for preselected word on [alt-tab] ctrl-C

    httpEnc.setHost("www.infoplease.com");
    connect(&httpEnc, SIGNAL(done(bool)), this, SLOT(slotSearchInfoDone(bool)));
    createLanguageMenu();
    //ui->retranslateUi(this); // changeEvent takes care
}

MainWindow::~MainWindow() {
    if (dataOk) {
        prop->setValue(OPTION_LAST_USED_DICTIONARY, dictionary->getCurrentDictionary());
        prop->setValue(OPTION_SHOW_TOOL_BAR, showToolBar);
        prop->setValue(OPTION_WATCH_CLIPBOARD, watchClipboard);
        if (!isMinimized()) {
                QString val = QString("%1, %2, %3, %4").arg(pos().x()).arg(pos().y()).arg(size().width()).arg(size().height());
                prop->setValue(OPTION_GEOMETRY, val);
        }

        delete dictionary;
        delete history;
//        if (bsearchDict != NULL) {
//                delete bsearchDict;
//                delete bsearchEncycl;
//                delete bsearchAll;
//        }
//        delete listOfDictionaries;
//        delete translations;
//        delete buttons;
//        delete panelButtons;

/*        for(int i = (int)filesHTable->count(); i >= 0; i--) {
                QString *qstrFile = filesHTable->find(i);
                filesHTable->remove(i);
                if (qstrFile != NULL) {
                        unlink(*qstrFile);
                        delete qstrFile;
                }
        } */
    }

    delete ui;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Context:
            minimizeAction->setEnabled(isVisible());
            maximizeAction->setEnabled(!isMaximized());
            restoreAction->setEnabled(isMaximized() || !isVisible());
            watchClipboardAction->setChecked(watchClipboard);
            break;
        case QSystemTrayIcon::Trigger:
            if(isVisible())
                hide();
            else {
                if(minimizeToSysTray && isMinimized() && !isMaximized())
                    showNormal(); // restore size & pos
                else
                    if(isMaximized())
                        showMaximized();  // win32 fix: showing maximized
                    else
                        show();
                raise();
                activateWindow();
            }
            break;
        //case QSystemTrayIcon::DoubleClick: break;
        case QSystemTrayIcon::MiddleClick:
            showTrayMessage();
            break;
        default:
            ;
    }
}

void MainWindow::showTrayMessage()
{
    trayIcon->showMessage(CAPTION, tr("Version: %1").arg(VERSION), QSystemTrayIcon::Information, 3000);
}

void MainWindow::messageClicked()
{
    //QMessageBox::information(0, tr(CAPTION), tr("Sorry, I already gave what help I could.\nMaybe you should try asking a human?"));
    on_action_About_triggered();
}

void MainWindow::changeEvent(QEvent *e) {
    if( e->type() == QEvent::WindowStateChange ){
         if( QSystemTrayIcon::isSystemTrayAvailable() &&
             sysTrayIcon && minimizeToSysTray && isMinimized() ){
             //hide();
             //setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
             // win32 Qt::WindowActive when hidden or minimized fails :(

             QTimer::singleShot(0, this, SLOT(hide())); // win32 fix for hiding minimized window
             //trayIcon->show(); // Always shown
             e->ignore();
             return;
         }
    }

    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            titleAction->setText(tr(CAPTION));
            watchClipboardAction->setText(tr(WATCH_CLIPBOARD_ACTION_TEXT));
            minimizeAction->setText(tr(MINIMIZE_ACTION_TEXT));
            maximizeAction->setText(tr(MAXIMIZE_ACTION_TEXT));
            restoreAction->setText(tr(RESTORE_ACTION_TEXT));
            quitAction->setText(tr(QUIT_ACTION_TEXT));
        }

        e->ignore();
        return;
    }

    QMainWindow::changeEvent(e);

/*
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
*/
}

bool MainWindow::isDataOk() {
        return dataOk;
}

void MainWindow::menu_Dict() {
    QAction  *act = qobject_cast<QAction*>(sender());
    if (act != 0){
        int id = act->data().toInt();
        dictionary->setCurrentDictionary(id);
        ui->action_Info->setIcon(act->icon());
        on_word_returnPressed();
    }
}

void MainWindow::Translations(TRANSLATIONS_TYPE tType) {
    //QString strLink;

    switch (tType) {
    // Lycos
        case LE2F:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=en_fr&partner=demo-Lycos2-en";
            break;
        case LE2G:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=en_de&partner=demo-Lycos2-en";
            break;
        case LE2I:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=en_it&partner=demo-Lycos2-en";
            break;
        case LE2P:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=en_pt&partner=demo-Lycos2-en";
            break;
        case LE2S:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=en_es&partner=demo-Lycos2-en";
            break;
        case LF2E:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=fr_en&partner=demo-Lycos2-en";
            break;
        case LF2G:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=fr_de&partner=demo-Lycos2-en";
            break;
        case LF2I:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=fr_it&partner=demo-Lycos2-en";
            break;
        case LF2P:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=fr_pt&partner=demo-Lycos2-en";
            break;
        case LF2S:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=fr_es&partner=demo-Lycos2-en";
            break;
        case LG2E:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=de_en&partner=demo-Lycos2-en";
            break;
        case LG2F:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=de_fr&partner=demo-Lycos2-en";
            break;
        case LI2E:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=it_en&partner=demo-Lycos2-en";
            break;
        case LI2F:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=it_fr&partner=demo-Lycos2-en";
            break;
        case LP2E:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=pt_en&partner=demo-Lycos2-en";
            break;
        case LS2E:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=es_en&partner=demo-Lycos2-en";
            break;
        case LS2F:
            strLink = "http://translate.lycoszone.com/lycos?urltext=$$$&systran_lp=es_fr&partner=demo-Lycos2-en";
            break;
    // Google
    // This format works only in browser "http://translate.google.com/#auto|bg|$$$"
        case GA2B:
            strLink = "http://translate.google.com/?sl=auto&tl=bg&q=$$$";
            break;
        case GA2E:
            strLink = "http://translate.google.com/?sl=auto&tl=en&q=$$$";
            break;
        case GB2E:
            strLink = "http://translate.google.com/?sl=bg&tl=en&q=$$$";
            break;
        case GB2F:
            strLink = "http://translate.google.com/?sl=bg&tl=fr&q=$$$";
            break;
        case GB2G:
            strLink = "http://translate.google.com/?sl=bg&tl=de&q=$$$";
            break;
        case GB2I:
            strLink = "http://translate.google.com/?sl=bg&tl=it&q=$$$";
            break;
        case GB2P:
            strLink = "http://translate.google.com/?sl=bg&tl=pt&q=$$$";
            break;
        case GB2R:
            strLink = "http://translate.google.com/?sl=bg&tl=ru&q=$$$";
            break;
        case GB2S:
            strLink = "http://translate.google.com/?sl=bg&tl=es&q=$$$";
            break;
        case GE2B:
            strLink = "http://translate.google.com/?sl=en&tl=bg&q=$$$";
            break;
        case GF2B:
            strLink = "http://translate.google.com/?sl=fr&tl=bg&q=$$$";
            break;
        case GG2B:
            strLink = "http://translate.google.com/?sl=de&tl=bg&q=$$$";
            break;
        case GI2B:
            strLink = "http://translate.google.com/?sl=it&tl=bg&q=$$$";
            break;
        case GP2B:
            strLink = "http://translate.google.com/?sl=pt&tl=bg&q=$$$";
            break;
        case GR2B:
            strLink = "http://translate.google.com/?sl=ru&tl=bg&q=$$$";
            break;
        case GS2B:
            strLink = "http://translate.google.com/?sl=es&tl=bg&q=$$$";
            break;
    // Multitran
        case ME2G:
            strLink = "http://www.multitran.ru/c/m.exe?l1=1&l2=3&s=$$$";
            break;
        case ME2J:
            strLink = "http://www.multitran.ru/c/m.exe?l1=1&l2=28&s=$$$";
            break;
        case ME2R:
            strLink = "http://www.multitran.ru/c/m.exe?l1=1&l2=2&s=$$$";
            break;
        case ME2Z:
            strLink = "http://www.multitran.ru/c/m.exe?l1=1&l2=26&s=$$$";
            break;
        case MR2A:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=31&s=$$$";
            break;
        case MR2D:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=24&s=$$$";
            break;
        case MR2E:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=1&s=$$$";
            break;
        case MR2F:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=4&s=$$$";
            break;
        case MR2G:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=3&s=$$$";
            break;
        case MR2I:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=23&s=$$$";
            break;
        case MR2J:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=28&s=$$$";
            break;
        case MR2K:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=35&s=$$$";
            break;
        case MR2L:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=27&s=$$$";
            break;
        case MR2S:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=5&s=$$$";
            break;
        case MR2X:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=34&s=$$$";
            break;
        case MR2Z:
            strLink = "http://www.multitran.ru/c/m.exe?l1=2&l2=26&s=$$$";
            break;
    // Wiktionary
        case W2B:
            strLink = "http://bg.wiktionary.org/wiki/$$$";
            break;
        case W2E:
            strLink = "http://en.wiktionary.org/wiki/$$$";
            break;
        case W2F:
            strLink = "http://fr.wiktionary.org/wiki/$$$";
            break;
        case W2G:
            strLink = "http://de.wiktionary.org/wiki/$$$";
            break;
        case W2I:
            strLink = "http://it.wiktionary.org/wiki/$$$";
            break;
        case W2P:
            strLink = "http://pt.wiktionary.org/wiki/$$$";
            break;
        case W2R:
            strLink = "http://ru.wiktionary.org/wiki/$$$";
            break;
        case W2S:
            strLink = "http://es.wiktionary.org/wiki/$$$";
            break;
    // Merriam-Webster  +http://www.m-w.com/cgi-bin/dictionary?book=Dictionary&va=$$$
        case MW:
            strLink = "http://www.merriam-webster.com/dictionary/$$$";
            break;
    // Longman Dictionary of Contemporary English
        case LDOCE:
            strLink = "http://www.ldoceonline.com/dictionary/$$$";
            break;
        default:
            return;
    }

    QString sTmp = ui->word->text();
    sTmp = QUrl::toPercentEncoding(sTmp);
    //strLink += sTmp; // Old Lycos style
    strLink.replace("$$$", sTmp);

    QUrl qUrl(strLink);
    if (!qUrl.isValid()) {
        //QMessageBox::critical(this, ui->centralWidget->windowTitle(), tr("Invalid URL.")); //kbgoffice vs kbgOA <2>
        QMessageBox::critical(this, this->windowTitle(), tr("Invalid URL."));
        return;
    }

    if (!qUrl.host().isEmpty())
        httpEnc.setHost(qUrl.host());

    // Google: Those formats works only in browser (or must implement some proto parsers)
    //   "http://translate.google.com/#auto|bg|$$$" "http://translate.google.com/translate_t#auto|bg|$$$" etc.

    // Longman doesn't accept words with spaces - must replace with '-'
    if(strLink.startsWith("http://www.ldoceonline.com/dictionary/"))
        strLink.replace("%20", QLatin1String("-"));
    else
        if(strLink.contains("wiktionary.org/"))
            strLink.replace("%20", QLatin1String("_"));

    // some ..e.g. Wiktionary want user-agent or simply wont work.
    QHttpRequestHeader header = QHttpRequestHeader("GET", strLink, 1, 1);
    header.setValue("Host", qUrl.host());
    header.setValue("User-Agent", "Mozilla/5.0");
    header.setValue("Connection", "Close");

    httpEnc.request(header); // httpEnc.get(strLink);
    ui->result->setPlainText(tr("Translating, please wait..."));
    ui->statusBar->showMessage(tr("Translating, please wait..."), 3000);
}

//Google
void MainWindow::on_actionAuto_to_Bulgarian_triggered()         { Translations(GA2B); }
void MainWindow::on_actionAuto_to_English_triggered()           { Translations(GA2E); }
void MainWindow::on_actionBulgarian_to_English_triggered()      { Translations(GB2E); }
void MainWindow::on_actionBulgarian_to_French_triggered()       { Translations(GB2F); }
void MainWindow::on_actionBulgarian_to_German_triggered()       { Translations(GB2G); }
void MainWindow::on_actionBulgarian_to_Italian_triggered()      { Translations(GB2I); }
void MainWindow::on_actionBulgarian_to_Portuguesse_triggered()  { Translations(GB2P); }
void MainWindow::on_actionBulgarian_to_Russian_triggered()      { Translations(GB2R); }
void MainWindow::on_actionBulgarian_to_Spanish_triggered()      { Translations(GB2S); }
void MainWindow::on_actionEnglish_to_Bulgarian_triggered()      { Translations(GE2B); }
void MainWindow::on_actionFrench_to_Bulgarian_triggered()       { Translations(GF2B); }
void MainWindow::on_actionGerman_to_Bulgarian_triggered()       { Translations(GG2B); }
void MainWindow::on_actionItalian_to_Bulgarian_triggered()      { Translations(GI2B); }
void MainWindow::on_actionPortuguese_to_Bulgarian_triggered()   { Translations(GP2B); }
void MainWindow::on_actionRussian_to_Bulgarian_triggered()      { Translations(GR2B); }
void MainWindow::on_actionSpanish_to_Bulgarian_triggered()      { Translations(GS2B); }

// Lycos
void MainWindow::on_actionEnglish_to_French_triggered()     { Translations(LE2F); }
void MainWindow::on_actionEnglish_to_German_triggered()     { Translations(LE2G); }
void MainWindow::on_actionEnglish_to_Italian_triggered()    { Translations(LE2I); }
void MainWindow::on_actionEnglish_to_Portuguese_triggered() { Translations(LE2P); }
void MainWindow::on_actionEnglish_to_Spanish_triggered()    { Translations(LE2S); }
void MainWindow::on_actionFrench_to_English_triggered()     { Translations(LF2E); }
void MainWindow::on_actionFrench_to_German_triggered()      { Translations(LF2G); }
void MainWindow::on_actionFrench_to_Italian_triggered()     { Translations(LF2I); }
void MainWindow::on_actionFrench_to_Portuguese_triggered()  { Translations(LF2P); }
void MainWindow::on_actionFrench_to_Spanish_triggered()     { Translations(LF2S); }
void MainWindow::on_actionGerman_to_English_triggered()     { Translations(LG2E); }
void MainWindow::on_actionGerman_to_French_triggered()      { Translations(LG2F); }
void MainWindow::on_actionItalian_to_English_triggered()    { Translations(LI2E); }
void MainWindow::on_actionItalian_to_French_triggered()     { Translations(LI2F); }
void MainWindow::on_actionPortuguese_to_English_triggered() { Translations(LP2E); }
void MainWindow::on_actionSpanish_to_English_triggered()    { Translations(LS2E); }
void MainWindow::on_actionSpanish_to_French_triggered()     { Translations(LS2F); }

// Multitran
void MainWindow::on_actionEnglish_to_GermanMT_triggered()   { Translations(ME2G); }
void MainWindow::on_actionEnglish_to_Japanese_triggered()   { Translations(ME2J); }
void MainWindow::on_actionEnglish_to_Russian_triggered()    { Translations(ME2R); }
void MainWindow::on_actionEnglish_to_Estonian_triggered()   { Translations(ME2Z); }
void MainWindow::on_actionRussian_to_Afrikaans_triggered()  { Translations(MR2A); }
void MainWindow::on_actionRussian_to_Dutch_triggered()      { Translations(MR2D); }
void MainWindow::on_actionRussian_to_English_triggered()    { Translations(MR2E); }
void MainWindow::on_actionRussian_to_French_triggered()     { Translations(MR2F); }
void MainWindow::on_actionRussian_to_German_triggered()     { Translations(MR2G); }
void MainWindow::on_actionRussian_to_Italian_triggered()    { Translations(MR2I); }
void MainWindow::on_actionRussian_to_Japanese_triggered()   { Translations(MR2J); }
void MainWindow::on_actionRussian_to_Kalmik_triggered()     { Translations(MR2K); }
void MainWindow::on_actionRussian_to_Latvian_triggered()    { Translations(MR2L); }
void MainWindow::on_actionRussian_to_Spanish_triggered()    { Translations(MR2S); }
void MainWindow::on_actionRussian_to_Estonian_triggered()   { Translations(MR2X); }
void MainWindow::on_actionRussian_to_Experanto_triggered()  { Translations(MR2Z); }

// Wiktionary
void MainWindow::on_actionIn_Bulgarian_triggered()          { Translations(W2B);  }
void MainWindow::on_actionIn_English_triggered()            { Translations(W2E);  }
void MainWindow::on_actionIn_French_triggered()             { Translations(W2F);  }
void MainWindow::on_actionIn_German_triggered()             { Translations(W2G);  }
void MainWindow::on_actionIn_Italian_triggered()            { Translations(W2I);  }
void MainWindow::on_actionIn_Portuguesse_triggered()        { Translations(W2P);  }
void MainWindow::on_actionIn_Russian_triggered()            { Translations(W2R);  }
void MainWindow::on_actionIn_Spanish_triggered()            { Translations(W2S);  }

// Merriam-Webster
void MainWindow::on_actionMeriam_Webster_triggered()        { Translations(MW); }

// Longman Dictionary of Contemporary English
void MainWindow::on_actionLongman_triggered()               { Translations(LDOCE); }

void MainWindow::on_action_Copy_triggered() {
    QTextCursor tc = ui->result->textCursor();

    if(tc.hasSelection()) // ui->result->hasFocus()
        ui->result->copy();
    else if(ui->word->hasSelectedText()) // ui->word->hasFocus()
        ui->word->copy();
// If QLineEdit has focus it processes c-ACV w/o passing over to MainWindow here.
}

void MainWindow::on_action_Paste_triggered() {
    fromClipboard = true;
    ui->word->setFocus();
    ui->word->clear();
    ui->word->paste();
}

void MainWindow::on_actionSelect_All_triggered() {
    if (ui->word->hasFocus()) {
            ui->word->selectAll();
    } else if (ui->result->hasFocus()) {
            ui->word->deselect();
            ui->result->selectAll();
    }
}

void MainWindow::on_actionC_lear_triggered() {
    ui->word->setFocus();
    ui->word->clear();
}

void MainWindow::on_bclear_clicked(){ on_actionC_lear_triggered(); }

void MainWindow::on_actionAdvanced_Search_triggered()
{
    ui->word->setFocus();
    ui->word->selectAll();
    if (dictionary->setAdvancedSearchText(U2W(ui->word->text()))) {
            addWordToHistory(U2W(ui->word->text()));
            int i = 0;
            ui->list->clear();
            while ((dictionary->searchNextWord()) && (i < wordsInList)) {
                    i++;
                    ui->list->addItem(W2U(dictionary->getWord()));
            }
            if (i > 0) {
                    fromSystemHighlighted = false;
                    fromAdvancedSearch = true;
                    ui->list->setCurrentItem(0);
            } else {
                    ui->result->clear();
            }
            ui->found->setStyleSheet(i == 0 ? COLOR_NOT_FOUND : COLOR_FOUND);
            ui->found->setText(QString("%1").arg(i));
    }
}

void MainWindow::on_bsearch_clicked(){ on_actionAdvanced_Search_triggered(); }

void MainWindow::on_actionSearch_in_Dic_tionary_triggered()
{
    QString qstrTmp(SEARCH_DICTIONARY);
    QString qTmp = ui->word->text();
    QUrl::toPercentEncoding(qTmp);
    qstrTmp += qTmp;
    QUrl qUrl(qstrTmp);
    if (!qUrl.host().isEmpty())
            httpEnc.setHost(qUrl.host());

    httpEnc.get( qstrTmp );
    ui->result->setPlainText(tr("Searching, please wait..."));
    ui->statusBar->showMessage(tr("Searching, please wait..."), 3000);
}

void MainWindow::on_bsearchDict_clicked(){ on_actionSearch_in_Dic_tionary_triggered(); }

void MainWindow::on_actionSearch_in_Encyclopedia_triggered()
{
    QString qstrTmp(SEARCH_ENCYCLOPEDIA);
    QString qTmp = ui->word->text();
    QUrl::toPercentEncoding(qTmp);
    qstrTmp += qTmp;
    QUrl qUrl(qstrTmp);
    if (!qUrl.host().isEmpty())
            httpEnc.setHost(qUrl.host());

    httpEnc.get(qstrTmp);
    ui->result->setPlainText(tr("Searching, please wait..."));
    ui->statusBar->showMessage(tr("Searching, please wait..."), 3000);
}

void MainWindow::on_bsearchEncycl_clicked(){ on_actionSearch_in_Encyclopedia_triggered(); }

void MainWindow::on_actionSearch_in_A_ll_triggered()
{
    QString qstrTmp(SEARCH_ALL);
    QString qTmp = ui->word->text();
    QUrl::toPercentEncoding(qTmp);
    qstrTmp += qTmp;
    QUrl qUrl(qstrTmp);
    if (!qUrl.host().isEmpty())
            httpEnc.setHost(qUrl.host());

    httpEnc.get(qstrTmp);
    ui->result->setPlainText(tr("Searching, please wait..."));
    ui->statusBar->showMessage(tr("Searching, please wait..."), 3000);
}

void MainWindow::on_bsearchAll_clicked(){ on_actionSearch_in_A_ll_triggered(); }

void MainWindow::addWordToHistory(const char *word){
    if (wordsInHistory > 0) {
        history->addItem(word);
        ui->actionHistory_Forward->setEnabled(false);
        ui->actionHistory_Back->setEnabled(true);
    }
}

void MainWindow::on_actionHistory_Back_triggered()
{
    if (history->isNew()) {
            history->prev();
            if (strcmp(history->getCurrentItem(), U2W(ui->word->text())) == 0) {
                    history->prev();
            }
    } else {
            if ((history->isFirst()) && (!ui->actionHistory_Forward->isEnabled())) {
                    if (strcmp(history->getCurrentItem(), U2W(ui->word->text())) == 0) {
                            history->prev();
                    }
            } else {
                    history->prev();
            }
    }
    if ((!ui->actionHistory_Forward->isEnabled()) && (!history->isFirst())) {
            ui->actionHistory_Forward->setEnabled(true);
    }
    if ((history->isLast()) && (history->getCurrentSize() > 1)) {
            ui->actionHistory_Back->setEnabled(false);
    }
    fromHistoryBuffer = true;
    ui->word->setFocus();
    ui->word->setText(W2U(history->getCurrentItem()));
    ui->word->selectAll();
}

void MainWindow::on_actionHistory_Forward_triggered()
{
    if (!ui->actionHistory_Back->isEnabled()) {
            ui->actionHistory_Back->setEnabled(true);
    }
    history->next();
    if (history->isFirst()) {
            ui->actionHistory_Forward->setEnabled(false);
    }
    fromHistoryBuffer = true;
    ui->word->setFocus();
    ui->word->setText(W2U(history->getCurrentItem()));
    ui->word->selectAll();
}

void MainWindow::on_actionHistory_Back_changed()
{
    QAction  *act = qobject_cast<QAction*>(sender());
    if (act != 0)
        ui->bprev->setEnabled(act->isEnabled());
}

void MainWindow::on_actionHistory_Forward_changed()
{
    QAction  *act = qobject_cast<QAction*>(sender());
    if (act != 0)
        ui->bnext->setEnabled(act->isEnabled());
}

void MainWindow::on_action_Show_ToolBar_triggered() {
    showToolBar = (!showToolBar);
    if (showToolBar) {
        if(dockToolBar) {
                ui->upperLayout->insertWidget(14, ui->mainToolBar, 0, Qt::AlignCenter);
            } else {
                this->addToolBar(ui->mainToolBar);
            }
        ui->mainToolBar->show();
    } else {
        ui->mainToolBar->hide();
    }
    ui->action_Show_ToolBar->setChecked(showToolBar);
}

void MainWindow::on_action_Quit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionA_bbreviations_triggered()
{
    char *c = NULL;
    c = dictionary->getAbbreviations();
    if (c[0] == '\0') {
            ui->result->setText(tr(MISSING_ABRR_FILE));
    } else {
            ui->result->setText(W2U(c));
    }
    ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}

void MainWindow::on_action_About_triggered()
{
    Ui::AboutDialog ui;
    QDialog about(this);
    ui.setupUi(&about);

    ui.message->setText(ui.message->text().arg(VERSION));
    ui.horizontalLayout->setSpacing(SPACE_IN_DIALOGS);
    ui.verticalLayout->setSpacing(SPACE_IN_DIALOGS);
//    QPixmap aboutPic(QString(dataDir) + "kbgoffice_about.png"); // built-in
//    ui.picture->setPixmap(aboutPic);
//    ui.picture->setAlignment(Qt::AlignTop);

    about.exec();
}

void MainWindow::on_action_Info_triggered()
{
    Ui::InfoDialog ui;
    QDialog info(this);
    ui.setupUi(&info);

    Dictionary d = dictionary->getDictionary(dictionary->getCurrentDictionary());
    char *p = dictionary->countWords();

    QPixmap dictIcon(d.icon.c_str());
    ui.dictPicture->setPixmap(dictIcon);
    ui.dictPicture->setAlignment(Qt::AlignTop);

    ui.message->setText(ui.message->text().arg(W2U(d.name.c_str())).arg(d.version.c_str()).arg(p).arg(dataDir));
    ui.horizontalLayout->setSpacing(SPACE_IN_DIALOGS);
    ui.verticalLayout->setSpacing(SPACE_IN_DIALOGS);

    info.exec();
}

void MainWindow::on_word_returnPressed()
{
    char *Translation = NULL;
    if ((!fromTextChanged) && (advSrchEnterSearch) &&
        ((ui->word->text()[0] == '+') || (ui->word->text()[0] == '"') || (ui->word->text()[0] == '-'))) {
            on_bsearch_clicked();
            return;
    }
    if (dictionary->getAdvancedSearchState()) {
            dictionary->setAdvancedSearchState(false);
    }
    if (dictionary->findWord(U2W(ui->word->text()), &Translation)) {
        if ( ui->word->text().toLower() == W2U(dictionary->getWord()).toLower() ) {
                    ui->found->setStyleSheet(COLOR_FOUND);
                    ui->found->setText(tr(FOUND));
            } else {
                    ui->found->setStyleSheet(COLOR_SIMILAR);
                    ui->found->setText(tr(SIMILAR));
            }
    } else {
            ui->found->setStyleSheet(COLOR_NOT_FOUND);
            ui->found->setText(tr(NOT_FOUND));
    }
    if (Translation[0] != '\0') {
            if ((strcmp(Translation, U2W(ui->result->toPlainText())) != 0) || (fromListSelected) || (fromSystem)) {
                    ui->result->setText(W2U(Translation));
                    ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
                    if (ui->list->isEnabled()) {
                            int i = 0;
                            ui->list->clear();
                            do {
                                    i++;
                                    ui->list->addItem(W2U(dictionary->getWord()));
                            } while ((dictionary->goToNextWord()) && (i < wordsInList));
                            fromSystemHighlighted = true;
                            fromAdvancedSearch = false;
                            ui->list->setCurrentRow(0, QItemSelectionModel::Select);
                            //ui->list->setCurrentItem(0); // Doesn't moveCursor(0)
                    }
            }
            if ((fromListSelected) || (fromHistoryBuffer) || (fromClipboard) || (fromSystem) || (!fromTextChanged)) {
                    ui->word->selectAll();
                    fromListSelected = false;
                    fromHistoryBuffer = false;
                    if (fromSystem) {
                            fromSystem = false;
                            fromTextChanged = true;
                    }
            }
            if ((!fromTextChanged) || (fromClipboard)) {
                    addWordToHistory(U2W(ui->word->text()));
                    fromClipboard = false;
            }
    } else {
            fromSystem = false;
    }
    fromTextChanged = false;
}

void MainWindow::on_word_textChanged(const QString &newText)
{
    if (fromListHighlighted) {
            fromListHighlighted = false;
            return;
    }
    if (newText.isEmpty())
        return;
    if ((newText[0] == '+') || (newText[0] == '"') || (newText[0] == '-')) {
            if ((!fromListSelected) && (!fromClipboard) && (!fromHistoryBuffer) && (advSrchIgnoreKey)) {
                    return;
            }
            if ((fromHistoryBuffer) && (advSrchIgnoreHistory)) {
                    fromHistoryBuffer = false;
                    return;
            }
    }
    if ((translateOnEveryKey) || (fromListSelected) || (fromHistoryBuffer) || (fromClipboard)) {
            fromTextChanged = true;
            on_word_returnPressed();
    }
}

void MainWindow::on_list_itemDoubleClicked(QListWidgetItem* item)
{
    fromListSelected = true;
    if (item){
        if(ui->word->text() != item->text()){
            ui->word->setText(item->text());
        } else {
            on_word_returnPressed();
        }
    }
}

void MainWindow::on_list_currentRowChanged(int currentRow)
{
    if(currentRow >= 0){
        QString newText = ui->list->item(currentRow)->text();
        char *Translation = NULL;
        if (fromSystemHighlighted) {
                fromSystemHighlighted = false;
        } else {
                dictionary->findWord(U2W(newText), &Translation);
                ui->result->setText(W2U(Translation));
                ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
                if (fromAdvancedSearch) {
                        fromAdvancedSearch = false;
                } else if ((!dictionary->getAdvancedSearchState()) || (!advSrchDoNotReplacePhrase)) {
                        fromListHighlighted = true;
                        ui->word->setText(newText);
                        ui->word->selectAll();
                }
        }
    }
}

void MainWindow::setView(QString style)
{
        qApp->setStyle(style);

        // Set font for screen element
        ui->word->setFont(font);
        ui->bclear->setFont(font);
        ui->bsearch->setFont(font);
        if (ui->bsearchDict != NULL) {
                ui->bsearchDict->setFont(font);
                ui->bsearchEncycl->setFont(font);
                ui->bsearchAll->setFont(font);
        }
        ui->bprev->setFont(font);
        ui->bnext->setFont(font);
        ui->found->setFont(font);
        ui->list->setFont(font);
        ui->result->setFont(font);

        // Update Word (text field)
        ui->word->setMaximumWidth(inputFieldWidth);
        ui->word->setMinimumWidth(inputFieldWidth);

        // Update Found label
        ui->found->setMaximumWidth(ui->word->height());
        ui->found->setMinimumWidth(ui->word->height());
        ui->found->setMaximumHeight(ui->word->height());
        ui->found->setMinimumHeight(ui->word->height());

        // Update buttons
        ui->bclear->setMaximumWidth(ui->bclear->height());
        ui->bclear->setMinimumWidth(ui->bclear->height());
        ui->bsearch->setMaximumWidth(ui->bclear->height());
        ui->bsearch->setMinimumWidth(ui->bclear->height());
        if (ui->bsearchDict != NULL) {
                ui->bsearchDict->setMaximumWidth(ui->bclear->height());
                ui->bsearchDict->setMinimumWidth(ui->bclear->height());
                ui->bsearchEncycl->setMaximumWidth(ui->bclear->height());
                ui->bsearchEncycl->setMinimumWidth(ui->bclear->height());
                ui->bsearchAll->setMaximumWidth(ui->bclear->height());
                ui->bsearchAll->setMinimumWidth(ui->bclear->height());
        }
        ui->bprev->setMaximumWidth(ui->bclear->height());
        ui->bprev->setMinimumWidth(ui->bclear->height());
        ui->bnext->setMaximumWidth(ui->bclear->height());
        ui->bnext->setMinimumWidth(ui->bclear->height());

        // List of dictionaries
        if (showToolBar) {
            if(dockToolBar) {
                ui->upperLayout->insertWidget(14, ui->mainToolBar, 0, Qt::AlignCenter);
                } else {
                    this->addToolBar(ui->mainToolBar);
                }
            ui->mainToolBar->show();
        } else {
            ui->mainToolBar->hide();
        }

        // Update list (of words)
        ui->list->setMaximumWidth(leftPanelWidth);
        ui->list->setMinimumWidth(leftPanelWidth);
        if ((leftPanelWidth > MIN_LEFT_PANEL_WIDTH) && (wordsInList > MIN_WORDS_IN_LIST)) {
                ui->list->setEnabled(true);
                ui->list->show();
        } else {
                ui->list->setEnabled(false);
                ui->list->hide();
        }

        // Update translation result
        ui->result->setText(ui->result->toPlainText());
        //ui->result->setHtml(ui->result->toHtml()); // Keeps colors:) and font:( settings
        ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}

void MainWindow::on_actionSelect_Font_triggered()
{
    bool okSelected;
    QFont f = QFont(font);
    f = QFontDialog::getFont(&okSelected, f, this);

    if (okSelected) {
        font = QFont(f);
        // Ignore some options which user may select
        font.setUnderline(false);
        font.setStrikeOut(false);
        // Save user selections
        prop->setValue(OPTION_FONT_NAME,   font.family()); //U2W(font.family()) | .toLocal8Bit().data()
        prop->setValue(OPTION_FONT_SIZE,   font.pointSize());
        prop->setValue(OPTION_FONT_WEIGHT, font.weight());
        prop->setValue(OPTION_FONT_ITALIC, font.italic());
        prop->sync();

        // Update view of main window
        setView("");
        // Update word and translation
        fromSystem = true;
        //on_word_returnPressed(); // doesn't work if Abbreviations shown
        // Quick fix
        if(!ui->result->toPlainText().contains(QString("Abbreviations").prepend('/')))
            on_word_returnPressed();
    }
}

void MainWindow::launchExternalBrowser(QString qstrFileName)
{
     QStringList arguments;
     arguments << qstrFileName;

    bool bSuccess = FALSE;
    QProcess qBrowser(this);
    bSuccess = qBrowser.startDetached(browserName, arguments);

    if (!bSuccess) {
            QMessageBox::critical(this, this->windowTitle(), tr("Failed to invoke: ") + browserName);
            return;
    }

    ui->result->setPlainText(tr("Done."));
    ui->statusBar->showMessage(tr("Done."), 3000);
}

void MainWindow::slotSearchInfoDone(bool bError)
{
    if (bError)
        QMessageBox::critical(this, this->windowTitle(), tr("Search failed. Reason:") + httpEnc.errorString());
    else {
        QByteArray qbtArr = httpEnc.readAll();
        if(qbtArr.isEmpty()) return; // probably setHost() fired

// If s.th. doesn't show as expected in QTextBrowser or browser use redirect (to real url) workaround below
/*        if(strLink.contains("google.com")) // Quick fix: some google urls work only in browser or must do some proto parsers here...
            qbtArr = QString("<HTML>"
                  "  <HEAD>"
                  "    <meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">"
                  "    <meta http-equiv=\"refresh\" content=\"0;url=%1\">"
                  "    <TITLE>301 Moved</TITLE>"
                  "  </HEAD>"
                  "  <BODY>"
                  "    <H1>301 Moved</H1>"
                  "    The document has moved"
                  "    <A HREF=\"%1\">here</A>."
                  "  </BODY>"
                  "</HTML>").arg(strLink).toUtf8(); */

        QTemporaryFile *file = new QTemporaryFile(this);
        // file.setFileTemplate("/tmp/SResXXXXXX");
        // Remember to change .arg() below: QDir::tempPath() vs. file.fileTemplate()
        if (file->open()) {
            file->write(qbtArr);
            file->close(); // this one just flushes file

            // On win32 Qt requires explicit protocol(s) e.g. "file:///"
            //  win32: ui->result->setSource() doesn't work with file:// only with file:///
            //  linux: some browsers (e.g. opera) handle :////url as ://0.0.0.0//url so replace
            QString strURI = QString("file:///").append(file->fileName()).replace("file:////", "file:///");
            if (useExternalBrowser)
                    launchExternalBrowser(strURI);
            else {
                    ui->result->setSource( QUrl(strURI) );
                    file->remove();
                    delete file;
            }
        } else {
            QMessageBox::critical(this, this->windowTitle(), W2U(CREATION_TEMP_FILE_FAILED).arg(QDir::tempPath()));
            return;
        }
    }
}

void MainWindow::on_result_anchorClicked(QUrl qUrl)
{
    if (!qUrl.isValid()) {
            QMessageBox::critical(this, this->windowTitle(), tr("Invalid URL.\n") + qUrl.toString());
            return;
    }
    if (!qUrl.host().isEmpty())
            httpEnc.setHost(qUrl.host());

    strLink="";
    httpEnc.get(qUrl.toString());
    ui->result->setPlainText(tr("Loading data, please wait..."));
    ui->statusBar->showMessage(tr("Loading data, please wait..."), 3000);
}

void MainWindow::word_UpDownPressed(int keyCode)
{
    uint uiCount = ui->list->count();
    if (uiCount <= 0)
        return;
    int iCurrentItem = ui->list->currentRow();
    if(keyCode == Qt::Key_Up){
        if (iCurrentItem > 0)
            ui->list->setCurrentRow(iCurrentItem - 1);
        else if (iCurrentItem == -1)
            ui->list->setCurrentItem(0);
    } else if(keyCode == Qt::Key_Down){
        if (iCurrentItem < (int)(uiCount - 1))
            ui->list->setCurrentRow(iCurrentItem + 1);
        else if (iCurrentItem == -1)
            ui->list->setCurrentRow((int)(uiCount - 1));
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->word) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if((keyEvent->key() == Qt::Key_Down) || (keyEvent->key() == Qt::Key_Up)){
                word_UpDownPressed(keyEvent->key());
                return true;
            } else
                return false;
        } else
            return false;
    } else if (obj == ui->list) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Enter)){
                ui->word->setText(ui->list->currentItem()->text());
                on_word_returnPressed();
                return true;
            } else
                return false;
        } else
            return false;
    } else
        return QMainWindow::eventFilter(obj, event);
}

void MainWindow::slotClipboardChanged(QClipboard::Mode mode)
{
    QString data;
    if( watchClipboard && advancedClipboard && systemClipboard->supportsSelection() ){
        if(mode == QClipboard::Selection )
            data = systemClipboard->text(QClipboard::Selection);
        else
            data = systemClipboard->text(QClipboard::Clipboard);
        if (!data.isNull()) {
                fromClipboard = true;
                ui->word->setFocus();
                ui->word->clear();
                ui->word->setText(data);
                activateWindow();
                raise();
        }
    }
}
/*
// Interprets everything as selection i.e. wont work for preselected word and ctrl-C = no sel. change
void MainWindow::slotSelectionChanged()
{
    if( systemClipboard->supportsSelection() && watchClipboard && advancedClipboard ){
        QString data = systemClipboard->text(QClipboard::Selection);
        if (!data.isNull()) {
                fromClipboard = true;
                ui->word->setFocus();
                ui->word->clear();
                ui->word->setText(data);
                activateWindow();
                raise();
        }
    }
}
*/
void MainWindow::slotClipboardChanged()
{
    if ((watchClipboard) && (!advancedClipboard)) {
            QString data = NULL;
            data = systemClipboard->text(QClipboard::Clipboard);
            if (!data.isNull()) {
                    fromClipboard = true;
                    ui->word->setFocus();
                    ui->word->clear();
                    ui->word->setText(data);
                    activateWindow();
                    raise();
            }
    }
}

void MainWindow::slotClipboardChanged(QString data)
{
    if (data != NULL) {
            fromClipboard = true;
            ui->word->setFocus();
            ui->word->clear();
            ui->word->setText(data);
            activateWindow();
            raise();
    }
}

void MainWindow::slotWatchClipboardOptionChanged()
{
    QAction  *wca = qobject_cast<QAction*>(sender());
    Q_CHECK_PTR(wca);
    if (wca->isChecked()) {
        watchClipboard = TRUE;
        //advancedClipboard = ?;
    } else {
        watchClipboard = FALSE;
        //advancedClipboard = FALSE;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (sysTrayIcon && closeToSysTray && trayIcon && trayIcon->isVisible()) {
/*        QMessageBox::information(this, tr(CAPTION),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));  */
            hide();
            event->ignore();
    } else {
        event->accept(); // Does nothing: Application::setQuitOnLastWindowClosed(false);
        QApplication::quit();
    }
}

void MainWindow::on_action_Configure_triggered()
{
    Ui::OptionsDialog odui;
    QDialog options(this);
    odui.setupUi(&options);

    odui.generalLayout->setSpacing(4); // SPACE_IN_DIALOGS - space needed for more options.
    odui.toek->setChecked(translateOnEveryKey);
    odui.sm->setChecked(dictionary->getSeparateMeanings());
    odui.dttmp->setChecked(dockToolBar);
    odui.act->setChecked(watchClipboard);
    odui.aact->setChecked(advancedClipboard);
    if (!odui.act->isChecked())
        odui.aact->setEnabled(FALSE);
    odui.uss->setChecked(useSplashScreen);
    odui.ist->setChecked(sysTrayIcon);
    odui.chs->setChecked(closeToSysTray);
    odui.mhs->setChecked(minimizeToSysTray);
    if (!odui.ist->isChecked()){
        odui.chs->setEnabled(FALSE);
        odui.mhs->setEnabled(FALSE);
    }

    //odui.subLayout->setSpacing(SPACE_IN_DIALOGS);
    odui.gridLayout->setSpacing(SPACE_IN_DIALOGS);
    odui.wih->setValue(wordsInHistory);
    odui.wil->setValue(wordsInList);
    odui.lpw->setValue(leftPanelWidth);
    odui.ifw->setValue(inputFieldWidth);
    odui.bw->setValue(borderWidth);
    odui.vspc1->changeSize(20, SPACE_IN_DIALOGS*2, QSizePolicy::Fixed, QSizePolicy::Fixed);
    odui.styleLayout->setSpacing(SPACE_IN_DIALOGS);

    QStringList::Iterator sty;
    QStringList styles = QStyleFactory::keys();
    styles.sort();
    for (sty = styles.begin(); sty != styles.end(); sty++) {
        odui.ss->addItem(*sty);
        if (*sty == systemStyleName) {
            odui.ss->setCurrentIndex(odui.ss->count() - 1);
        }
    }
    odui.vspc2->changeSize(20, SPACE_IN_DIALOGS*2, QSizePolicy::Fixed, QSizePolicy::Fixed);

    odui.advancedSearchLayout->setSpacing(SPACE_IN_DIALOGS);
    odui.sww->setChecked(dictionary->getAdvancedSearchWholeWord());
    odui.sep->setChecked(dictionary->getAdvancedSearchExactPhrase());
    odui.chr->setChecked(dictionary->getAdvancedSearchHighlight());
    odui.itoek->setChecked(advSrchIgnoreKey);
    odui.iht->setChecked(advSrchIgnoreHistory);
    odui.dnrp->setChecked(advSrchDoNotReplacePhrase);
    odui.ewas->setChecked(advSrchEnterSearch);
    odui.vspc3->changeSize(20, SPACE_IN_DIALOGS*2, QSizePolicy::Fixed, QSizePolicy::Fixed);

    odui.languagesLayout->setSpacing(SPACE_IN_DIALOGS);
    odui.tws->setChecked(dictionary->getTryWithoutSuffix());
    odui.vspc4->changeSize(20, SPACE_IN_DIALOGS*2, QSizePolicy::Fixed, QSizePolicy::Fixed);
    odui.uao->setChecked(advancedOptions);
    odui.vspc5->changeSize(20, SPACE_IN_DIALOGS*2, QSizePolicy::Fixed, QSizePolicy::Fixed);
    odui.ueb->setChecked(useExternalBrowser);
    odui.bn->setText(browserName);
    odui.bn->setMaxLength(PATH_MAX);

    // Execute dialog and check for OK button
    if (options.exec() == QDialog::Accepted) {
        // Save and apply new values
        if (translateOnEveryKey != odui.toek->isChecked()) {
                translateOnEveryKey = odui.toek->isChecked();
                prop->setValue(OPTION_TRANSLATE_ON_EVERY_KEY, translateOnEveryKey);
        }
        if (dictionary->getSeparateMeanings() != odui.sm->isChecked()) {
                dictionary->setSeparateMeanings(odui.sm->isChecked());
                prop->setValue(OPTION_SEPARATE_MEANINGS, odui.sm->isChecked());
        }
        if (dockToolBar != odui.dttmp->isChecked()) {
                dockToolBar = odui.dttmp->isChecked();
                prop->setValue(OPTION_DOCK_TOOL_BAR, dockToolBar);
        }
        if (systemClipboard->supportsSelection() && (advancedClipboard != odui.aact->isChecked())) {
                advancedClipboard = odui.aact->isChecked();
                prop->setValue(OPTION_ADVANCED_CLIPBOARD, advancedClipboard);
        }
        if (watchClipboard != odui.act->isChecked()) {
                watchClipboard = odui.act->isChecked();
        }

        if (sysTrayIcon != odui.ist->isChecked()) {
                sysTrayIcon = odui.ist->isChecked();
                prop->setValue(OPTION_ICON_IN_SYSTRAY, sysTrayIcon);
                if (QSystemTrayIcon::isSystemTrayAvailable()){
                    if(sysTrayIcon)
                        trayIcon->show();
                    else
                        trayIcon->hide();
                }
        }
        if (closeToSysTray != odui.chs->isChecked()) {
                closeToSysTray = odui.chs->isChecked();
                prop->setValue(OPTION_CLOSE_HIDE2_SYSTRAY, closeToSysTray);
        }
        if (minimizeToSysTray != odui.mhs->isChecked()) {
                minimizeToSysTray = odui.mhs->isChecked();
                prop->setValue(OPTION_MINIMIZE_TO_SYSTRAY, minimizeToSysTray);
        }

        if (useSplashScreen != odui.uss->isChecked()) {
                useSplashScreen = odui.uss->isChecked();
                prop->setValue(OPTION_USE_SPLASH_SCREEN, useSplashScreen);
        }
        if (inputFieldWidth != odui.ifw->value()) {
                inputFieldWidth = odui.ifw->value();
                prop->setValue(OPTION_INPUT_FIELD_WIDTH, inputFieldWidth);
        }
        if (borderWidth != odui.bw->value()) {
                borderWidth = odui.bw->value();
                prop->setValue(OPTION_BORDER_WIDTH, borderWidth);
        }
        if (leftPanelWidth != odui.lpw->value()) {
                leftPanelWidth = odui.lpw->value();
                prop->setValue(OPTION_LEFT_PANEL_WIDTH, leftPanelWidth);
        }
        if (wordsInList != odui.wil->value()) {
                wordsInList = odui.wil->value();
                prop->setValue(OPTION_WORDS_IN_LIST, wordsInList);
        }
        if (wordsInHistory != odui.wih->value()) {
                wordsInHistory = odui.wih->value();
                prop->setValue(OPTION_WORDS_IN_HISTORY, wordsInHistory);
                history->setMaxSize(wordsInHistory);
                if (wordsInHistory == 0) {
                    ui->bnext->setEnabled(false);
                    ui->bprev->setEnabled(false);
                }
        }
        if (systemStyleName != odui.ss->currentText()) {
                systemStyleName = odui.ss->currentText();
                prop->setValue(OPTION_SYSTEM_STYLE_NAME, systemStyleName); //U2W()
        }

        // Advanced Search
        if (dictionary->getAdvancedSearchWholeWord() != odui.sww->isChecked()) {
                dictionary->setAdvancedSearchWholeWord(odui.sww->isChecked());
                prop->setValue(OPTION_ADV_SRCH_WHOLE_WORD, odui.sww->isChecked());
        }
        if (dictionary->getAdvancedSearchExactPhrase() != odui.sep->isChecked()) {
                dictionary->setAdvancedSearchExactPhrase(odui.sep->isChecked());
                prop->setValue(OPTION_ADV_SRCH_EXACT_PHRASE, odui.sep->isChecked());
        }
        if (dictionary->getAdvancedSearchHighlight() != odui.chr->isChecked()) {
                dictionary->setAdvancedSearchHighlight(odui.chr->isChecked());
                prop->setValue(OPTION_ADV_SRCH_HIGHLIGHT, odui.chr->isChecked());
        }
        if (advSrchIgnoreKey != odui.itoek->isChecked()) {
                advSrchIgnoreKey = odui.itoek->isChecked();
                prop->setValue(OPTION_ADV_SRCH_ASD_IGNORE_KEY, advSrchIgnoreKey);
        }
        if (advSrchIgnoreHistory != odui.iht->isChecked()) {
                advSrchIgnoreHistory = odui.iht->isChecked();
                prop->setValue(OPTION_ADV_SRCH_ASD_IGNORE_HIS, advSrchIgnoreHistory);
        }
        if (advSrchDoNotReplacePhrase != odui.dnrp->isChecked()) {
                advSrchDoNotReplacePhrase = odui.dnrp->isChecked();
                prop->setValue(OPTION_ADV_SRCH_ASD_DO_NOT_REPL, advSrchDoNotReplacePhrase);
        }
        if (advSrchEnterSearch != odui.ewas->isChecked()) {
                advSrchEnterSearch = odui.ewas->isChecked();
                prop->setValue(OPTION_ADV_SRCH_ASD_ENTER, advSrchEnterSearch);
        }

        // Languages
        if (dictionary->getTryWithoutSuffix() != odui.tws->isChecked()) {
                dictionary->setTryWithoutSuffix(odui.tws->isChecked());
                prop->setValue(OPTION_TRY_WITHOUT_SUFFIX, odui.tws->isChecked());
        }

        // Translation
        if (advancedOptions != odui.uao->isChecked()) {
            advancedOptions = odui.uao->isChecked();
            prop->setValue(OPTION_ADVANCED_OPTIONS, advancedOptions);
            if(advancedOptions){
                ui->menuBar->insertAction(ui->menu_Settings->menuAction(), ui->menu_Translation->menuAction());
                ui->menu_Edit->insertAction(ui->actionHistory_Back, ui->actionSearch_in_Dic_tionary);
                ui->menu_Edit->insertAction(ui->actionHistory_Back, ui->actionSearch_in_Encyclopedia);
                ui->menu_Edit->insertAction(ui->actionHistory_Back, ui->actionSearch_in_A_ll);
                ui->bsearchDict->show();
                ui->bsearchEncycl->show();
                ui->bsearchAll->show();
            } else {
                ui->menuBar->removeAction(ui->menu_Translation->menuAction());
                ui->menu_Edit->removeAction(ui->actionSearch_in_Dic_tionary);
                ui->menu_Edit->removeAction(ui->actionSearch_in_Encyclopedia);
                ui->menu_Edit->removeAction(ui->actionSearch_in_A_ll);
                ui->bsearchDict->hide();;
                ui->bsearchEncycl->hide();
                ui->bsearchAll->hide();
            }
        }

        if (useExternalBrowser != odui.ueb->isChecked()) {
                useExternalBrowser = odui.ueb->isChecked();
                prop->setValue(OPTION_USE_EXTERNAL_BROWSER, useExternalBrowser);
        }

        if (!odui.bn->text().isEmpty()) {
                browserName = odui.bn->text();
                prop->setValue(OPTION_DEFAULT_BROWSER, browserName);
        }

        if(!systemClipboard->supportsSelection())
            advancedClipboard = FALSE;

        prop->sync();

        // Update view of main window
        setView(systemStyleName);
        // Update word and translation
        fromSystem = true;
        on_word_returnPressed();
    }
}

void MainWindow::on_action_Test_triggered()
{
    TestDialog test(prop, dictionary, &font, borderWidth, this);

    QPushButton *bcloze= test.ui->bclose->button( QDialogButtonBox::Close );
    Q_CHECK_PTR(bcloze);
    if(bcloze) bcloze->setAutoDefault(false);

    QPushButton *bhelp= test.ui->bclose->button( QDialogButtonBox::Help );
    Q_CHECK_PTR(bhelp);
    if(bhelp) bhelp->setAutoDefault(false);

    test.exec();
}

void MainWindow::on_actionWhat_s_This_triggered()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr(CAPTION));
}

void MainWindow::switchLanguage(QAction *action)
{
    QString locale = action->data().toString();
//    QDir dir(QApplication::applicationDirPath());
//    QString qmPath = dir.absolutePath();

    qtTranslator.load("qt_" + locale, dataDir);
    if(appTranslator.load("kbgoffice_" + locale, dataDir)){
        prop->setValue(OPTION_INTERFACE_LANGUAGE, locale);
        prop->sync();
    }
    //ui->retranslateUi(this); //changeEvent takes care
}

void MainWindow::createLanguageMenu()
{
    QActionGroup *languageActionGroup = new QActionGroup(this);
    languageActionGroup->setExclusive(true);
    connect(languageActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(switchLanguage(QAction *)));

    QDir qmDir(dataDir);
    QStringList fileNames = qmDir.entryList(QStringList("kbgoffice_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);

        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());
        //: This refers to language translating to e.g. Deutsch - NOT English!
        QString language = translator.translate("MainWindow", INTERFACE_LANG);

        QAction *action = new QAction(tr("&%1. %2").arg(i + 1).arg(language), this); // languageActionGroup)
        action->setCheckable(true);
        action->setData(locale);

        languageActionGroup->addAction(action);
        ui->menu_Interface->addActions(languageActionGroup->actions());

        //: This refers to language translating to e.g. Deutsch - NOT English!
        if( language == qApp->translate("MainWindow", INTERFACE_LANG) )
            action->setChecked(true);
    }
}
