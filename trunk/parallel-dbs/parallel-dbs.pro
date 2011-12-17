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
    offsetrule.cpp \
    matchtonothingrule.cpp \
    superscriptionwlcrule.cpp \
    syncnumberlookup.cpp \
    headingsrule.cpp
HEADERS += versereference.h \
    globalvariables.h \
    dbupdater.h \
    rule.h \
    standardrule.h \
    offsetrule.h \
    matchtonothingrule.h \
    superscriptionwlcrule.h \
    syncnumberlookup.h \
    headingsrule.h
