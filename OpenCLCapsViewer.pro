TEMPLATE = app
TARGET = OpenCLCapsViewer
QT       += core network gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
DEFINES += GUI_BUILD
DEFINES += QT_DLL QT_NETWORK_LIB QT_WIDGETS_LIB
DEFINES += CL_TARGET_OPENCL_VERSION=300
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    displayutils.cpp \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    deviceinfo.cpp \
    openclfunctions.cpp \
    openclinfo.cpp \
    platforminfo.cpp \
    treeproxyfilter.cpp \
    submitdialog.cpp \
    settings.cpp \
    settingsdialog.cpp \
    appinfo.cpp \
    report.cpp \
    operatingsystem.cpp

HEADERS += \
    displayutils.h \
    mainwindow.h \
    database.h \
    deviceinfo.h \
    openclfunctions.h \
    openclinfo.h \
    platforminfo.h \
    treeproxyfilter.h \
    submitdialog.h \
    settings.h \
    settingsdialog.h \
    appinfo.h \
    report.h \
    operatingsystem.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += "external/OpenCL-Headers"

win32 {
}

linux:!android {
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
    LIBS += -ldl
    INSTALLS += target
    desktop.files = OpenCLCapsViewer.desktop
    desktop.path = /usr/share/applications
    icon.extra = cp $$PWD/Resources/icon.png openclCapsViewer.png
    icon.files = openclCapsViewer.png
    icon.path = /usr/share/icons/hicolor/256x256/apps/
    INSTALLS += desktop icon    
}

android {
    QT += androidextras
    CONFIG += mobility
    MOBILITY =
    LIBS += -landroid

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    DISTFILES += \
        android/AndroidManifest.xml \
        android/build.gradle \
        android/res/values/libs.xml
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mainwindow.qrc

win32:RC_FILE = OpenCLCapsViewer.rc
