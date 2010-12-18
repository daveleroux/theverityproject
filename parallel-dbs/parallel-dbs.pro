# -------------------------------------------------
# Project created by QtCreator 2010-09-09T19:13:24
# -------------------------------------------------
QT += sql
QT -= gui
TARGET = parallel-dbs
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    versereference.cpp \
    globalvariables.cpp \
    dbupdater.cpp \
    rule.cpp \
    standardrule.cpp \
    gen32wlcrule.cpp \
    jn1tischrule.cpp
HEADERS += versereference.h \
    globalvariables.h \
    dbupdater.h \
    rule.h \
    standardrule.h \
    gen32wlcrule.h \
    jn1tischrule.h
