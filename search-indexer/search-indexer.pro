#-------------------------------------------------
#
# Project created by QtCreator 2011-12-29T19:02:57
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = search-indexer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include

LIBS += -L/usr/lib -lclucene

SOURCES += main.cpp
