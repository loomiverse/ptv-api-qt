#-------------------------------------------------
#
# Project created by QtCreator 2014-03-13T19:58:04
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = ptvTimetable
TEMPLATE = lib
CONFIG += staticlib

SOURCES += ptvtimetable.cpp

HEADERS += ptvtimetable.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += QJson
