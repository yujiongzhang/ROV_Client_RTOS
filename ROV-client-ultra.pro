QT       += core gui network gamepad

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11 console

LIBS += -lws2_32

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
    aspectratiowidget.cpp \
    axbopenglwidget.cpp \
    crc8_crc16.cpp \
    definition.cpp \
    help.cpp \
    joystickrule.cpp \
    main.cpp \
    mainwindow.cpp \
    msg.cpp \
    pidsets.cpp \
    qcustomplot.cpp \
    qmotorlineedit.cpp \
    recodefile.cpp \
    sendfile.cpp \
    sonarset.cpp \
    startplot.cpp \
    threadsafe_queue.cpp \
    zhifan.cpp


HEADERS += \
    aspectratiowidget.h \
    axbopenglwidget.h \
    crc8_crc16.h \
    definition.h \
    help.h \
    joystickrule.h \
    mainwindow.h \
    msg.h \
    pidsets.h \
    qcustomplot.h \
    qmotorlineedit.h \
    recodefile.h \
    sendfile.h \
    sonarset.h \
    startplot.h \
    threadsafe_queue.h \
    zhifan.h \

FORMS += \
    help.ui \
    joystickrule.ui \
    mainwindow.ui \
    pidsets.ui \
    sonarset.ui \
    startplot.ui \
    zhifan.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH += $$PWD/RtspCamera
include ($$PWD/RtspCamera/RtspCamera.pri)

DISTFILES +=

RESOURCES += \
    res.qrc
