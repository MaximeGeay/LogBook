QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network positioning xml

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    anaarchicont.cpp \
    anaprofils.cpp \
    customtablemodel.cpp \
    depouillement.cpp \
    eventdetails.cpp \
    eventlistmodel.cpp \
    eventmanager.cpp \
    eventsettings.cpp \
    fenmission.cpp \
    fenpreferences.cpp \
    logbookmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    navdata.cpp \
    udpdata.cpp

HEADERS += \
    anaarchicont.h \
    anaprofils.h \
    customtablemodel.h \
    depouillement.h \
    eventdetails.h \
    eventlistmodel.h \
    eventmanager.h \
    eventsettings.h \
    fenmission.h \
    fenpreferences.h \
    logbookmodel.h \
    mainwindow.h \
    navdata.h \
    udpdata.h

FORMS += \
    anaarchicont.ui \
    anaprofils.ui \
    depouillement.ui \
    eventdetails.ui \
    eventmanager.ui \
    eventsettings.ui \
    fenmission.ui \
    fenpreferences.ui \
    mainwindow.ui \
    navdata.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
