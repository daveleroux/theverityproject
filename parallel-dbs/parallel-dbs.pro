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
    jn1tischrule.cpp \
    mt12tischrule.cpp \
    offsetrule.cpp \
    matchtonothingrule.cpp \
    tischheadingsrule.cpp \
    wlcheadingsrule.cpp \
    mt5v4tischrule.cpp \
    secondco13v13tischrule.cpp
HEADERS += versereference.h \
    globalvariables.h \
    dbupdater.h \
    rule.h \
    standardrule.h \
    gen32wlcrule.h \
    jn1tischrule.h \
    mt12tischrule.h \
    offsetrule.h \
    matchtonothingrule.h \
    tischheadingsrule.h \
    wlcheadingsrule.h \
    mt5v4tischrule.h \
    secondco13v13tischrule.h
