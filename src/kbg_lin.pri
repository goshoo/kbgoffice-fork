unix{
    isEmpty(DATADIR):DATADIR = "/usr/local/share/bgoffice/"
    isEmpty(INSTALLDIR):INSTALLDIR = "/usr/local/bin/"

    #kde-config --path icon #/usr/share/icons/
    #/usr/share/icons/hicolor/32x32/apps/
    exists(/usr/share/icons/hicolor/32x32/apps/):ICONDIR="/usr/share/icons/hicolor/32x32/apps/"
    !isEmpty(ICONDIR){
        icondata.path = $$ICONDIR
        icondata.files = images/kbgoffice_icon.png
        INSTALLS += icondata
    }

    QMAKE_CLEAN += config.h #clean00
    QMAKE_EXTRA_TARGETS += clean0 config0
    PRE_TARGETDEPS += config.h #clean00

    clean0.target = clean00
    clean0.commands = rm -rf config.h #& echo "" > $$clean0.target

    config0.target = config.h
    config0.depends = clean0
    config0.commands = \ #rm -rf $$clean0.target ; \
#        echo -e "\"$${LITERAL_HASH}ifndef CONFIG_H\n$${LITERAL_HASH}define CONFIG_H\n\n$${LITERAL_HASH}define PACKAGE \\\"$${PACKAGE}\\\"\n$${LITERAL_HASH}define VERSION \\\"$${VERSION}\\\"\n$${LITERAL_HASH}define DATA_DIR_CONFIG \\\"$${DATADIR}\\\"\n\n$${LITERAL_HASH}endif // CONFIG_H\"" > $$config0.target
     echo "\"$${LITERAL_HASH}ifndef CONFIG_H\"" > $$config0.target ; \
     echo "\"$${LITERAL_HASH}define CONFIG_H\"" >> $$config0.target ; \
     echo "" >> $$config0.target ; \
     echo "\"$${LITERAL_HASH}define PACKAGE \\\"$${PACKAGE}\\\"\"" >> $$config0.target ; \
     echo "\"$${LITERAL_HASH}define VERSION \\\"$${VERSION}\\\"\"" >> $$config0.target ; \
     echo "\"$${LITERAL_HASH}define DATA_DIR_CONFIG \\\"$${DATADIR}\\\"\"" >> $$config0.target ; \
     echo "" >> $$config0.target ; \
     echo "\"$${LITERAL_HASH}endif // CONFIG_H\"" >> $$config0.target

    !isEmpty(CREATEDESKTOPLINKS):!contains(CREATEDESKTOPLINKS,0){
        QMAKE_CLEAN += desktop01 kbgoffice.desktop
        QMAKE_EXTRA_TARGETS += desktop0 desktop
        PRE_TARGETDEPS += kbgoffice.desktop #desktop00

        desktop0.target = desktop00
        desktop0.commands = rm -rf kbgoffice.desktop # & echo "" > $${desktop0.target}

        desktop.target = kbgoffice.desktop
        desktop.depends = desktop0
        desktop.commands = \ # rm -rf $${desktop0.target} \
         echo "\"\"$${LITERAL_HASH}!/usr/bin/env xdg-open\"\"" > $$desktop.target ; \
         echo "\"[Desktop Entry]\"" >> $$desktop.target ; \
         echo "\"Version=1.0\"" >> $$desktop.target ; \
         echo "\"Type=Application\"" >> $$desktop.target ; \
         echo "\"Name=KBGOffice\"" >> $$desktop.target ; \
         echo "\"Name[bg]=КБГОфис\"" >> $$desktop.target ; \
         echo "\"Comment=BG Office Assistant\"" >> $$desktop.target ; \
         echo "\"Comment[bg]=БГ Офис помощник\"" >> $$desktop.target ; \
         echo "\"TryExec=kbgoffice\"" >> $$desktop.target ; \
         echo "\"Exec=kbgoffice %F\"" >> $$desktop.target ; \
         echo "\"Icon=$${DATADIR}kbgoffice_icon.png\"" >> $$desktop.target ; \
         #!isEmpty(ICONDIR):echo "\"Icon=$${ICONDIR}kbgoffice_icon.png\"" >> $$desktop.target ; \
         echo "\"Encoding=UTF-8\"" >> $$desktop.target ; \
         echo "\"GenericName=BG Office Assistant\"" >> $$desktop.target ; \
         echo "\"GenericName[bg]=БГ Офис помощник\"" >> $$desktop.target ; \
         echo "\"Categories=Office;KDE;Qt;Utility;Dictionary;\"" >> $$desktop.target ; \
         echo "\"Terminal=false\"" >> $$desktop.target

        deskinst.path = ~/Desktop
        deskinst.files = $$desktop.target

        deskins2.path = $$DATADIR
        deskins2.files = kbgoffice.desktop

        INSTALLS += deskinst deskins2
    }

    dictdata.path = $$DATADIR
    dictdata.files = data/*.*
    dictdata.extra = chmod -R 644 data/*.*

    qmdata.path = $$DATADIR
    qmdata.files = images/kbgoffice_icon.png *.qm

    target.path = $$INSTALLDIR
    INSTALLS += target dictdata qmdata

    # Some distros build release with debbug info
    CONFIG(release, debug|release):QMAKE_POST_LINK=strip $(TARGET)
}
