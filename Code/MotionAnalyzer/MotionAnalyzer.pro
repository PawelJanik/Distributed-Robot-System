QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += mqtt sql

CONFIG += c++11 console
CONFIG -= app_bundle

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
        ../lib/daemon.cpp \
        ../lib/device.cpp \
        ../lib/robot.cpp \
        ../lib/sensor.cpp \
        main.cpp \
        motionanalyzer.cpp

DISTFILES += \
    MotionAnalyzer-r1.service \
    motion-analyzer.conf


# Default rules for deployment.

service.files = MotionAnalyzer-r1.service
service.path = /lib/systemd/system

config.files = motion-analyzer.conf
config.path = /etc/drs/r1

target.path += /usr/bin

INSTALLS += target service config

HEADERS += \
    ../lib/daemon.h \
    ../lib/device.h \
    ../lib/robot.h \
    ../lib/sensor.h \
    confData.h \
    motionanalyzer.h
