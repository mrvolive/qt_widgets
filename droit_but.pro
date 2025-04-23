QT       += core gui network positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    view/mapwidget.cpp \
    model/placemodel.cpp \
    model/mapmodel.cpp \
    controller/searchcontroller.cpp \
    controller/mapcontroller.cpp

HEADERS += \
    mainwindow.h \
    view/mapwidget.h \
    model/placemodel.h \
    model/mapmodel.h \
    controller/searchcontroller.h \
    controller/mapcontroller.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

