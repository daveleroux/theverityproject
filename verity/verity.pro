# -------------------------------------------------
# Project created by QtCreator 2010-03-03T18:41:14
# -------------------------------------------------
QT += sql \
    xmlpatterns \
    webkit
TARGET = verity
TEMPLATE = app

INCLUDEPATH += /usr/include/clucene09
LIBS += -L/usr/lib64/clucene09 -lclucene

####
# Bunch of old paths that should be removed
#INCLUDEPATH += /Users/dave/Downloads/clucene-core-2.3.3.4/src/shared
#INCLUDEPATH += /Users/dave/Downloads/clucene-core-2.3.3.4/src/core
#INCLUDEPATH += /usr/lib64/clucene09 # except maybe this
#LIBS += -L/Users/dave/Downloads/clucene-core-2.3.3.4/bin/Debug -lclucene-core -lclucene-shared
#LIBS += -L/usr/lib -lclucene # and this?
####

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
    textspecificdata.cpp \
    biblequerier.cpp \
    verselocation.cpp \
    chapterdisplayer.cpp \
    singletextchapterdisplayer.cpp \
    paralleltextchapterdisplayer.cpp \
    parallelchapterrepresentation.cpp \
    versenode.cpp \
    parallelgridconstructor.cpp \
    referencefilterproxymodel.cpp \
    minandmaxidsforchapter.cpp \
    scrolllistener.cpp \
    paralleldto.cpp \
    eventmanager.cpp \
    event.cpp \
    wordclickedevent.cpp \
    listener.cpp \
    javascriptclicklistener.cpp \
    wordclickedlistener.cpp \
    wordhandler.cpp \
    strongswordhandler.cpp \
    parsingwordhandler.cpp \
    strongsevent.cpp \
    parsingevent.cpp \
    netnotebrowser.cpp \
    netnoteevent.cpp \
    grid.cpp \
    vwebview.cpp \
    basicevent.cpp \
    vbibleinterface.cpp \
    vlocationedit.cpp \
    vlocationdropdowns.cpp \
    vlocationlineedit.cpp \
    biblereferenceevent.cpp \
    emptyparsingwordhandler.cpp \
    vwebhistory.cpp \
    vwebhistoryitem.cpp \
    basicwebhistoryitem.cpp \
    searchdisplaybrowser.cpp \
    searchevent.cpp \
    netanalyzer.cpp
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
    textspecificdata.h \
    biblequerier.h \
    verselocation.h \
    chapterdisplayer.h \
    singletextchapterdisplayer.h \
    paralleltextchapterdisplayer.h \
    parallelchapterrepresentation.h \
    versenode.h \
    parallelgridconstructor.h \
    referencefilterproxymodel.h \
    minandmaxidsforchapter.h \
    locationlineedit.h \
    scrolllistener.h \
    paralleldto.h \
    eventmanager.h \
    event.h \
    wordclickedevent.h \
    listener.h \
    javascriptclicklistener.h \
    wordclickedlistener.h \
    wordhandler.h \
    strongswordhandler.h \
    parsingwordhandler.h \
    strongsevent.h \
    parsingevent.h \
    netnotebrowser.h \
    netnoteevent.h \
    grid.h \
    vwebview.h \
    basicevent.h\
    vbibleinterface.h \
    vlocationedit.h \
    vlocationdropdowns.h \
    vlocationlineedit.h \
    biblereferenceevent.h \
    emptyparsingwordhandler.h \
    vwebhistory.h \
    vwebhistoryitem.h \
    basicwebhistoryitem.h \
    searchdisplaybrowser.h \
    searchevent.h \
    netanalyzer.h
INSTALLS += target \
    datatarget
target.path = /usr/bin
datatarget.path = /usr/share/verity/
datatarget.files += verity.sqlite
datatarget.files += style.xsl
datatarget.files += hebrew-style.xsl
datatarget.files += bible.css
