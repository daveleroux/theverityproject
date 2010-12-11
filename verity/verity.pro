# -------------------------------------------------
# Project created by QtCreator 2010-03-03T18:41:14
# -------------------------------------------------
QT += sql \
    xmlpatterns
TARGET = verity
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    basetextunit.cpp \
    bibletextbrowser.cpp \
    textinfo.cpp \
    versereferenceparser.cpp \
    versereference.cpp \
    parseattribute.cpp \
    parsingdecoder.cpp \
    morphattribute.cpp \
    parsingdisplaybrowser.cpp \
    documentrepresentation.cpp \
    chapterrepresentation.cpp \
    globalvariables.cpp \
    qsnapscrollbar.cpp \
    markedscrollbar.cpp \
    dictionarybrowser.cpp \
    textspecificdata.cpp
HEADERS += mainwindow.h \
    basetextunit.h \
    bibletextbrowser.h \
    textinfo.h \
    versereferenceparser.h \
    versereference.h \
    parseattribute.h \
    parsingdecoder.h \
    morphattribute.h \
    parsingdisplaybrowser.h \
    timer.h \
    documentrepresentation.h \
    chapterrepresentation.h \
    globalvariables.h \
    qsnapscrollbar.h \
    markedscrollbar.h \
    dictionarybrowser.h \
    textspecificdata.h
INSTALLS += target \
    datatarget
target.path = /usr/bin
datatarget.path = /usr/share/verity/
datatarget.files += bibles.sqlite
datatarget.files += style.xsl
datatarget.files += hebrew-style.xsl