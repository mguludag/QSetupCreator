QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32-msvc* {
    RC_ICONS = icon.ico
    win32:RC_FILE = proj.rc

}

win32-g++ {
    RC_ICONS = icon.ico
    CONFIG += embed_manifest_exe
    win32:RC_FILE = proj.rc

}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    dialog.hpp \
    mainwindow.hpp \
    settings.hpp \
    version.h

FORMS += \
    dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    icon.qrc
