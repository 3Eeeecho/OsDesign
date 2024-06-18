QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bankersalgorithm.cpp \
    labfive.cpp \
    labfour.cpp \
    labone.cpp \
    labseven.cpp \
    labsix.cpp \
    labthree.cpp \
    labtwo.cpp \
    main.cpp \
    home.cpp

HEADERS += \
    bankersalgorithm.h \
    home.h \
    labfive.h \
    labfour.h \
    labone.h \
    labseven.h \
    labsix.h \
    labthree.h \
    labtwo.h

FORMS += \
    home.ui \
    labtwo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
