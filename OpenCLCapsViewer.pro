QT       += core network gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DLL QT_NETWORK_LIB QT_WIDGETS_LIB

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    displayutils.cpp \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    deviceinfo.cpp \
    platforminfo.cpp \
    treeproxyfilter.cpp \
    submitdialog.cpp

HEADERS += \
    displayutils.h \
    mainwindow.h \
    database.h \
    deviceinfo.h \
    platforminfo.h \
    treeproxyfilter.h \
    submitdialog.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += "OpenCL-Headers"

win32 {
# todo
    LIBS += "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\lib\x64\OpenCL.lib"
}

linux:!android {
    LIBS += -lOpenCL
    contains(DEFINES, X11) {
        message("Building for X11")
        QT += x11extras
        DEFINES += VK_USE_PLATFORM_XCB_KHR
    }
    contains(DEFINES, WAYLAND) {
        message("Building for Wayland")
        QT += waylandclient
        DEFINES += VK_USE_PLATFORM_WAYLAND_KHR
    }
    target.path = /usr/bin
    INSTALLS += target
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mainwindow.qrc

win32:RC_FILE = OpenCLCapsViewer.rc
