#-------------------------------------------------
#
# Project created by QtCreator 2010-03-03T15:56:41
#
#-------------------------------------------------

QT       += sql xml

QT       -= gui

TARGET = tisch-importer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    book.cpp \
    chapter.cpp \
    verse.cpp \
    word.cpp

HEADERS += \
    book.h \
    chapter.h \
    verse.h \
    word.h
