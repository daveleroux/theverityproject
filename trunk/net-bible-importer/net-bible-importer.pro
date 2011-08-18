#-------------------------------------------------
#
# Project created by QtCreator 2011-07-10T22:10:03
#
#-------------------------------------------------

QT       += sql xml

QT       -= gui

TARGET = net-bible-importer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    chunk.cpp \
    note.cpp

HEADERS += \
    chunk.h \
    note.h
