QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    enemy.cpp \
    fireattack.cpp \
    gordo.cpp \
    hothead.cpp \
    kirby.cpp \
    main.cpp \
    gamewindow.cpp \
    platform.cpp \
    portal.cpp \
    slopeplatform.cpp \
    sparky.cpp \
    starbullet.cpp \
    waddledee.cpp

HEADERS += \
    enemy.h \
    fireattack.h \
    gamewindow.h \
    gordo.h \
    hothead.h \
    item.h \
    kirby.h \
    platform.h \
    portal.h \
    slopeplatform.h \
    sparky.h \
    starbullet.h \
    waddledee.h

FORMS += \
    gamewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
