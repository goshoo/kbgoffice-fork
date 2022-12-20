# If you wish data files to be installed - Before running make
# extract/put them in ./data (without subfolders)

# Do not remove quotes around values
# To put " use \" To put \ use \\ To put \" use \\\"
# To put # use $${LITERAL_HASH}
# Always put ending path delimiter

unix{
    DATADIR  = "/usr/local/share/bgoffice/"
    INSTALLDIR  = "/usr/local/bin/"
}

win32{
    DATADIR = "."
    INSTALLDIR = "%PROGRAMFILES%\\bgoffice\\"

    # If limited users can't write to %PROGRAMFILES%
    # use one of alternative paths below or supply your own
    #INSTALLDIR = "C:\\Programs\\bgoffice\\"
    #INSTALLDIR = "C:\\bgoffice\\"
    #DATADIR    = "C:\\bgoffice\\"
}

# Use only 0 or 1 here.
CREATEDESKTOPLINKS = "1"

# If you change parameter(s) above do 'make clean' and 'make' or Build:Re/Build All

### User configurable section ends here!!!
### Please do not edit anything below this line!!!

PACKAGE = "bgoffice"
VERSION = "2.1"

#QT += webkit
QT += network
QT += widgets
TARGET = kbgoffice
TEMPLATE = app

DEPENDPATH += .
INCLUDEPATH += .
SOURCES += application.cpp \
    database.cpp \
    history.cpp \
    main.cpp \
    mainwindow.cpp \
    properties.cpp \
    translator.cpp \
    translator_manager.cpp \
    testdialog.cpp
HEADERS += application.h \
    config.h \
    database.h \
    encodechar.h \
    history.h \
    mainwindow.h \
    properties.h \
    translator.h \
    translator_manager.h \
    testdialog.h
FORMS += about.ui \
    info.ui \
    mainwindow.ui \
    options.ui \
    testdialog.ui \
    testoptions.ui

macx:ICON = ./icons/macicon.icns
symbian:ICON = ./icons/s60icon.svg
win32:RC_FILE = ./icons/winicon.rc

RESOURCES += kbgoffice.qrc
TRANSLATIONS += kbgoffice_en.ts kbgoffice_bg.ts

include( kbg_lin.pri )
include( kbg_win.pri )
kbgoffice.depends = config0 desktop
