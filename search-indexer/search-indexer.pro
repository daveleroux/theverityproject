#-------------------------------------------------
#
# Project created by QtCreator 2011-12-29T19:02:57
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = search-indexer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include
INCLUDEPATH += /Users/dave/Downloads/clucene-core-2.3.3.4/src/shared
INCLUDEPATH += /Users/dave/Downloads/clucene-core-2.3.3.4/src/core


LIBS += -L/usr/lib -lclucene
#LIBS += -L/Users/dave/Downloads/clucene-core-2.3.3.4/bin/Debug -lclucene-core -lclucene-shared

SOURCES += main.cpp \
    netanalyzer.cpp

HEADERS += \
    netanalyzer.h
