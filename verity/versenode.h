#ifndef VERSENODE_H
#define VERSENODE_H

#include "textinfo.h"
#include <QList>

class VerseNode
{
private:
    void init();

public:
    int parallelId;
    QList<TextInfo> textInfos;

    VerseNode* right;
    VerseNode* left;
    VerseNode* down;
    VerseNode* up;

    VerseNode();
    VerseNode(int parallelId);
    ~VerseNode();

    void addTextInfo(TextInfo textInfo);
    void insertBelowMe(VerseNode* verseNode);
    void setRightOfMe(VerseNode* verseNode);
};

#endif // VERSENODE_H
