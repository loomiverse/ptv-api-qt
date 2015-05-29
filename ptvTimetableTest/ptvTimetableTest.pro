#-------------------------------------------------
#
# Project created by QtCreator 2014-03-13T19:58:58
#
#-------------------------------------------------

QT       += testlib network

QT       -= gui

TARGET = tst_ptvtimetabletest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_ptvtimetabletest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ptvTimetable/release/ -lptvTimetable
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ptvTimetable/debug/ -lptvTimetable
else:unix: LIBS += -L$$OUT_PWD/../ptvTimetable/ -lptvTimetable

INCLUDEPATH += $$PWD/../ptvTimetable
DEPENDPATH += $$PWD/../ptvTimetable

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ptvTimetable/release/ptvTimetable.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ptvTimetable/debug/ptvTimetable.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ptvTimetable/libptvTimetable.a

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += QJson
