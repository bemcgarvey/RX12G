QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HidUSBLink.cpp \
    QHidWatcher.cpp \
    RX12G.cpp \
    bootloader.cpp \
    bootloaderusblink.cpp \
    gyrodisplay.cpp \
    hexfile.cpp \
    hidbootloader.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow2.cpp \
    minmaxbar.cpp \
    qhorizonwidget.cpp \
    version.cpp \
    workerthread.cpp

HEADERS += \
    HidUSBLink.h \
    QHidWatcher.h \
    RX12G.h \
    bootloader.h \
    bootloaderusblink.h \
    gyrodisplay.h \
    hexfile.h \
    hidbootloader.h \
    mainwindow.h \
    minmaxbar.h \
    qhorizonwidget.h \
    version.h \
    workerthread.h

FORMS += \
    mainwindow.ui


win32: LIBS += -lhid
win32: LIBS += -lsetupapi
win32: LIBS += -luser32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32:RC_ICONS += images/radio.ico
