QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adminmanagerdialog.cpp \
    adminvarifydialog.cpp \
    applicationdialog.cpp \
    begindialog.cpp \
    chatdialog.cpp \
    clickedbtn.cpp \
    clickedlabel.cpp \
    customizeedit.cpp \
    editinfodialog.cpp \
    favoritedialog.cpp \
    givemoneydialog.cpp \
    global.cpp \
    homedialog.cpp \
    httpmanager.cpp \
    kimiapi.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    menudialog.cpp \
    merchantappdialog.cpp \
    merchantdialog.cpp \
    merchantmanager.cpp \
    orderdialog.cpp \
    ordersmanager.cpp \
    particleeffect.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    scancodedialog.cpp \
    settingdialog.cpp \
    tcpmanager.cpp \
    timerbtn.cpp \
    usermanager.cpp \
    backgrounddialog.cpp

HEADERS += \
    adminmanagerdialog.h \
    adminvarifydialog.h \
    applicationdialog.h \
    begindialog.h \
    chatdialog.h \
    clickedbtn.h \
    clickedlabel.h \
    customizeedit.h \
    editinfodialog.h \
    favoritedialog.h \
    givemoneydialog.h \
    global.h \
    homedialog.h \
    httpmanager.h \
    kimiapi.h \
    logindialog.h \
    mainwindow.h \
    menudialog.h \
    merchantappdialog.h \
    merchantdialog.h \
    merchantmanager.h \
    orderdialog.h \
    ordersmanager.h \
    particleeffect.h \
    registerdialog.h \
    resetdialog.h \
    scancodedialog.h \
    settingdialog.h \
    singleton.h \
    tcpmanager.h \
    timerbtn.h \
    usermanager.h \
    backgrounddialog.h

FORMS += \
    adminmanagerdialog.ui \
    adminvarifydialog.ui \
    applicationdialog.ui \
    begindialog.ui \
    chatdialog.ui \
    editinfodialog.ui \
    favoritedialog.ui \
    givemoneydialog.ui \
    homedialog.ui \
    logindialog.ui \
    mainwindow.ui \
    menudialog.ui \
    merchantappdialog.ui \
    merchantdialog.ui \
    orderdialog.ui \
    registerdialog.ui \
    resetdialog.ui \
    scancodedialog.ui \
    settingdialog.ui

LIBS += -lcurl -ljsoncpp -lzbar

# OpenCV configuration
LIBS += -L/usr/local/lib \
        -lopencv_objdetect \
        -lopencv_videoio \
        -lopencv_imgcodecs \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_core

INCLUDEPATH += /usr/local/include/opencv4

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    resources.qrc

DISTFILES += \
    CMakeLists.txt \
    HungerYet.pro.user \
    package.sh
