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

BibleTextBrowser::BibleTextBrowser() : QWebView()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);

    //    int size = settings.beginReadArray(FONT_FAMILY_SETTINGS);
    //    for (int i = 0; i < size; i++)
    //    {
    //        settings.setArrayIndex(i);
    //        QString text = settings.value(TEXT_SETTING).toString();
    //        QString fontFamily = settings.value(FONT_SETTING).toString();
    //        fontFamilies.insert(text, fontFamily);
    //    }
    //    settings.endArray();

    //    if (size == 0)
    //    {
    //        fontFamilies.insert("esv", "DejaVuSans");
    //        fontFamilies.insert("kjv", "DejaVuSans");
    //        fontFamilies.insert("tisch", "DejaVuSans");
    //        fontFamilies.insert("wlc", "SBL Hebrew");
    //    }

    //    if (!settings.contains("zoomIn"))
    //    {
    //        settings.setValue("zoomIn", 1);
    //    }

    //    int zoomInSetting = settings.value("zoomIn", 1).toInt();

    //    settings.endGroup();


    chapterDisplayer = 0;

    page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
}

void BibleTextBrowser::writeOutSettings()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);

    //    settings.beginWriteArray(FONT_FAMILY_SETTINGS);
    //    for (int i = 0; i < fontFamilies.size(); i++)
    //    {
    //        settings.setArrayIndex(i);
    //        settings.setValue(TEXT_SETTING, fontFamilies.keys().at(i));
    //        settings.setValue(FONT_SETTING, fontFamilies.value(fontFamilies.keys().at(i)));
    //    }
    //    settings.endArray();

    settings.endGroup();
}

void BibleTextBrowser::display(QList<int> bibletextIds, int idLocation, int normalisedChapterLocation)
{
    if(chapterDisplayer != 0)
    {        
        delete chapterDisplayer;
    }

    if(bibletextIds.size() > 1)
    {
        chapterDisplayer = new ParallelTextChapterDisplayer(this, bibletextIds);
    }
    else
    {
        chapterDisplayer = new SingleTextChapterDisplayer(this, bibletextIds);
    }

    connect(chapterDisplayer, SIGNAL(wordClicked(TextInfo*)), this, SIGNAL(wordClicked(TextInfo*)));

    chapterDisplayer->display(idLocation, normalisedChapterLocation);
}

void BibleTextBrowser::display(QList<int> bibletextIds, VerseReference verseReference)
{
    if(bibletextIds.size() > 0)
    {
        VerseLocation* newVerseLocation = BibleQuerier::getVerseLocation(bibletextIds.at(0), verseReference);

        if(newVerseLocation != 0)
        {
            int idLocation = newVerseLocation->id;
            int normalisedChapterLocation = newVerseLocation->normalisedChapter;
            delete newVerseLocation;

            window()->setWindowTitle(PROGRAM_NAME + " - " + verseReference.stringRepresentation);

            display(bibletextIds, idLocation, normalisedChapterLocation);
        }
        else
        {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(verseReference.stringRepresentation + " could not be found in the primary text");
            msgBox.exec();
        }
    }
}


//void BibleTextBrowser::resizeEvent(QResizeEvent* event)
//{
//    QWebView::resizeEvent(event);
//    emit checkCanScrollSignal(0);
//}


//void BibleTextBrowser::scrollbarValueChanged(int v)
//{
//    chapterDisplayer->checkCanScroll();
//}



//QMimeData* BibleTextBrowser::createMimeDataFromSelection() const
//{
//    QMimeData* mimeData = new QMimeData();
//    mimeData->setText(textCursor().selectedText());
//    return mimeData;
//}
