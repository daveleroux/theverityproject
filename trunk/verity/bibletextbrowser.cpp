#include <QDebug>
#include <QtSql>

#include <QTextCursor>
#include <QApplication>
#include <QMessageBox>

#include "bibletextbrowser.h"
#include "timer.h"
#include "globalvariables.h"
#include "biblequerier.h"
#include "paralleltextchapterdisplayer.h"
#include "singletextchapterdisplayer.h"

#include <iostream>
using namespace std;

BibleTextBrowser::BibleTextBrowser() : QTextBrowser()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);

    int size = settings.beginReadArray(FONT_FAMILY_SETTINGS);
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        QString text = settings.value(TEXT_SETTING).toString();
        QString fontFamily = settings.value(FONT_SETTING).toString();
        fontFamilies.insert(text, fontFamily);
    }
    settings.endArray();

    if (size == 0)
    {
        fontFamilies.insert("esv", "DejaVuSans");
        fontFamilies.insert("kjv", "DejaVuSans");
        fontFamilies.insert("tisch", "DejaVuSans");
        fontFamilies.insert("wlc", "SBL Hebrew");
    }

    if (!settings.contains("zoomIn"))
    {
        settings.setValue("zoomIn", 1);
    }

    int zoomInSetting = settings.value("zoomIn", 1).toInt();

    settings.endGroup();


    setUndoRedoEnabled(false);
    setMouseTracking(true);
    zoomIn(zoomInSetting);



    markedScrollBar = new QSnapScrollBar(this);
    setVerticalScrollBar(markedScrollBar);

    sliderBeingPressed = false;
    ignoreSliderValueChanges = false;
    resizing = false;

    connect(markedScrollBar, SIGNAL(sliderPressed()), this, SLOT(scrollbarSliderPressed()));
    connect(markedScrollBar, SIGNAL(sliderReleased()), this, SLOT(scrollbarSliderReleased()));

    connect(markedScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollbarValueChanged(int)));
    connect(this, SIGNAL(checkCanScrollSignal(int)), this, SLOT(scrollbarValueChanged(int)), Qt::QueuedConnection);

    connect(this, SIGNAL(chapterStarts(QList<int>)), markedScrollBar, SLOT(defineSnapPoints(QList<int>)));
    connect(this, SIGNAL(chapterStarts(QList<int>)), this, SLOT(tmp(QList<int>)));

    chapterDisplayer = 0;
}

void BibleTextBrowser::writeOutSettings()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);

    settings.beginWriteArray(FONT_FAMILY_SETTINGS);
    for (int i = 0; i < fontFamilies.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue(TEXT_SETTING, fontFamilies.keys().at(i));
        settings.setValue(FONT_SETTING, fontFamilies.value(fontFamilies.keys().at(i)));
    }
    settings.endArray();

    settings.endGroup();
}

void BibleTextBrowser::display(QList<QString> texts, int idLocation, int normalisedChapterLocaction)
{
    ignoreSliderValueChanges = true;

    document()->clear();

    if(chapterDisplayer != 0)
    {
        delete chapterDisplayer;
    }

    if(texts.size() > 1)
    {
        chapterDisplayer = new ParallelTextChapterDisplayer(this, texts, fontFamilies);
    }
    else
    {
        chapterDisplayer = new SingleTextChapterDisplayer(this, texts, fontFamilies);
    }

    connect(chapterDisplayer, SIGNAL(wordClicked(TextInfo*)), this, SIGNAL(wordClicked(TextInfo*)));
    connect(chapterDisplayer, SIGNAL(chapterStarts(QList<int>)), this, SIGNAL(chapterStarts(QList<int>)));

    chapterDisplayer->display(idLocation, normalisedChapterLocaction);

    ignoreSliderValueChanges = false;
}

void BibleTextBrowser::display(QList<QString> texts, VerseReference verseReference)
{
    if(texts.size() > 0)
    {
        VerseLocation* newVerseLocation = BibleQuerier::getVerseLocation(texts.at(0), verseReference);

        if(newVerseLocation != 0)
        {
            int idLocation = newVerseLocation->id;
            int normalisedChapterLocation = newVerseLocation->normalisedChapter;
            delete newVerseLocation;

            window()->setWindowTitle(PROGRAM_NAME + " - " + verseReference.stringRepresentation);

            display(texts, idLocation, normalisedChapterLocation);
        }
        else
        {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(verseReference.stringRepresentation + " could not be found in the " + texts.at(0));
            msgBox.exec();
        }
    }
}


void BibleTextBrowser::mouseMoveEvent ( QMouseEvent * e )
{
    QTextBrowser::mouseMoveEvent(e);
}

void BibleTextBrowser::mousePressEvent(QMouseEvent* e)
{
    if(chapterDisplayer != 0)
        chapterDisplayer->mousePressed(e->pos());
    QTextBrowser::mousePressEvent(e);
}

void BibleTextBrowser::resizeEvent(QResizeEvent* event)
{
        resizing =  true;
         QTextBrowser::resizeEvent(event);
        resizing = false;
        emit checkCanScrollSignal(0);
}

void BibleTextBrowser::tmp(QList<int> pixelStarts)
{
    markedScrollBar->removeAllMarks();

    for(int i=0;i<pixelStarts.size();i++)
    {
        if(i!=0)
            markedScrollBar->addMark(pixelStarts.at(i),QColor(Qt::black),"");
    }
}

void BibleTextBrowser::scrollbarValueChanged(int v)
{
    static bool busy = false;

    if(!busy && !resizing && !ignoreSliderValueChanges && chapterDisplayer != 0 && !sliderBeingPressed)
    {
        busy = true;
        chapterDisplayer->checkCanScroll();
        busy = false;
    }
}
void BibleTextBrowser::scrollbarSliderPressed()
{
    sliderBeingPressed = true;
}

void BibleTextBrowser::scrollbarSliderReleased()
{
    sliderBeingPressed = false;
    scrollbarValueChanged(0);
}

//void BibleTextBrowser::wheelEvent ( QWheelEvent * e )
//{
//    QTextBrowser::wheelEvent(e);
//    chapterDisplayer->checkCanScroll();
//}


QMimeData* BibleTextBrowser::createMimeDataFromSelection() const
{
    QMimeData* mimeData = new QMimeData();
    mimeData->setText(textCursor().selectedText());
    return mimeData;
}
