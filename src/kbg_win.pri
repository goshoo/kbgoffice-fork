win32{
    isEmpty(DATADIR):DATADIR = "."
    isEmpty(INSTALLDIR):INSTALLDIR = "C:\\bgoffice\\"

    contains(DATADIR,"."):DATADIR = $$INSTALLDIR
    DATADIR = "\"$$system(echo $${DATADIR})\"" # expand %PROGRAMFILES%
    DATADIRCONFIG = $$DATADIR
    DATADIRCONFIG = $$replace(DATADIRCONFIG, "\\\\", "/")
    # config.h : Qt path delimtier is always /

    INSTALLDIRVBS = $$INSTALLDIR
    INSTALLDIR = "\"$$system(echo $$INSTALLDIR)\"" # INSTALLS doesn't like %PROGRAMFILES%

    QMAKE_CLEAN += config.h #clean01
    QMAKE_EXTRA_TARGETS += clean0 config0
    PRE_TARGETDEPS += config.h #clean01

    # To prevent 'mingw32-make clean' every time invent autoclean :)
    clean0.target = clean01
    clean0.commands = del /f config.h #& echo. > $$clean0.target

    config0.target = config.h
    config0.depends = clean0
    config0.commands = \ #del /f $${clean0.target} & \
       echo $${LITERAL_HASH}ifndef CONFIG_H >  $$config0.target \
     & echo $${LITERAL_HASH}define CONFIG_H >> $$config0.target \
     & echo. >> $$config0.target \
     & echo $${LITERAL_HASH}define PACKAGE \"$${PACKAGE}\" >> $$config0.target \
     & echo $${LITERAL_HASH}define VERSION \"$${VERSION}\" >> $$config0.target \
     & echo. >> $$config0.target \
     & echo $${LITERAL_HASH}define DATA_DIR_CONFIG $${DATADIRCONFIG} >> $$config0.target \
     & echo. >> $$config0.target \
     & echo $${LITERAL_HASH}endif // CONFIG_H >> $$config0.target

    !isEmpty(CREATEDESKTOPLINKS):!contains(CREATEDESKTOPLINKS,0){
        QMAKE_CLEAN += desktop01 desktop.vbs kbgoffice.lnk
        QMAKE_EXTRA_TARGETS += desktop0 desktop
        PRE_TARGETDEPS += desktop.vbs #desktop01

        desktop0.target = desktop01
        desktop0.commands = del /f desktop.vbs & del /f kbgoffice.lnk # & echo. > $${desktop0.target}

        desktop.target = desktop.vbs
        desktop.depends = desktop0
        desktop.commands = \ # del /f $${desktop0.target} & \
           echo Set Shell = CreateObject(\"WScript.Shell\") > $$desktop.target \
         #& echo DesktopPath = Shell.SpecialFolders(\"AllUsersDesktop\") >> $$desktop.target \
         & echo DesktopPath = Shell.SpecialFolders(\"Desktop\") >> $$desktop.target \
         & echo Set link = Shell.CreateShortcut(DesktopPath ^& \"\\kbgoffice.lnk\") >> $$desktop.target \
         #\' link.Arguments = \"1 2 3\"
         & echo link.Description = \"kbgoffice\" >> $$desktop.target \
         & echo link.HotKey = \"CTRL+ALT+SHIFT+K\" >> $$desktop.target \
         #& echo link.IconLocation = \"kbgoffice.exe,1\" >> $$desktop.target \
         & echo link.IconLocation = \"$${INSTALLDIRVBS}kbgoffice.exe\" >> $$desktop.target \
         & echo link.TargetPath = \"$${INSTALLDIRVBS}kbgoffice.exe\" >> $$desktop.target \
         & echo link.WindowStyle = 1 >> $$desktop.target \
         & echo link.WorkingDirectory = \"$${INSTALLDIRVBS}\" >> $$desktop.target \
         & echo link.Save >> $$desktop.target
         #& cscript $$desktop.target //Nologo # use install target deskinst below

        deskinst.path = . # mandatory to create target
        deskinst.extra = cscript $$desktop.target //Nologo
        INSTALLS += deskinst
    }

    MINGWBIN = $$system(echo %PATH%)
    MINGWBIN = $$replace(MINGWBIN, ";", " ")
    # Replace \ with \\ to fix 'Unescaped backslashes are deprecated'
    MINGWBIN = $$replace(MINGWBIN, \\\\, \\\\)
    eval("MINGWBIN = $$MINGWBIN")
    MINGWBIN = $$find(MINGWBIN, .*mingw\\\\bin.*)
    MINGWBIN = $$member(MINGWBIN, 0)
    MINGWBIN2 = "$${MINGWBIN}\\"

    # With those is compiled Qt stuff - ours might differ!!!
    #MINGWDLL  = $$[QT_INSTALL_BINS]/mingwm10.dll
    #LIBGCCDLL = $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll
    # NB! Above are used by Qt bins - ours *might* differ /!= MinGW install/

    MINGWDLL = $$[QT_INSTALL_PREFIX]@
    MINGWDLL = $$replace(MINGWDLL, qt@, )mingw/bin/mingwm10.dll
    !exists($$MINGWDLL){
        MINGWDLL = c:/mingw/bin/mingwm10.dll
        !exists($$MINGWDLL){
            MINGWDLL = mingwm10.dll
            !exists($$MINGWDLL){
                MINGWDLL = $${MINGWBIN}mingwm10.dll
                !exists($$MINGWDLL){
                    MINGWDLL = $${MINGWBIN2}mingwm10.dll
                    !exists($$MINGWDLL):MINGWDLL = ""
                }
            }
        }
    }
    isEmpty(MINGWDLL):message(" mingwm10.dll not found - please copy in *.pro dir and try again.")

    LIBGCCDLL = $$[QT_INSTALL_PREFIX]@
    LIBGCCDLL = $$replace(LIBGCCDLL, qt@, )mingw/bin/libgcc_s_dw2-1.dll
    !exists($$LIBGCCDLL){
        LIBGCCDLL = c:/mingw/bin/libgcc_s_dw2-1.dll
        !exists($$LIBGCCDLL){
            LIBGCCDLL = libgcc_s_dw2-1.dll
            !exists($$LIBGCCDLL){
                LIBGCCDLL = $${MINGWBIN}libgcc_s_dw2-1.dll
                !exists($$LIBGCCDLL){
                    LIBGCCDLL = $${MINGWBIN2}libgcc_s_dw2-1.dll
                    !exists($$LIBGCCDLL):LIBGCCDLL = ""
                }
            }
        }
    }
    isEmpty(LIBGCCDLL):message(" libgcc_s_dw2-1.dll not found - please copy in *.pro dir and try again.")
    #message(MinGW: $$MINGWDLL $$LIBGCCDLL)

    mingw.path  = $$INSTALLDIR
    mingw.files = $$MINGWDLL $$LIBGCCDLL

    qtdlls.path  = $$INSTALLDIR
    CONFIG(release, debug|release):qtdlls.files = $$[QT_INSTALL_BINS]/QtCore4.dll $$[QT_INSTALL_BINS]/QtGui4.dll $$[QT_INSTALL_BINS]/QtNetwork4.dll
    CONFIG(debug, debug|release):qtdlls.files = $$[QT_INSTALL_BINS]/QtCored4.dll $$[QT_INSTALL_BINS]/QtGuid4.dll $$[QT_INSTALL_BINS]/QtNetworkd4.dll

    dictdata.path = $$DATADIR
    dictdata.files = data\\*.*
    # NB! Never use / here i.e. data/*.* - works but crashes $$DATADIR if LFN

    qmdata.path = $$DATADIR
    qmdata.files = *.qm

    target.path = $$INSTALLDIR
    INSTALLS += target mingw qtdlls dictdata qmdata

    #release:CXXFLAGS += -DNDEBUG
}
