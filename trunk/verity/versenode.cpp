#include "versenode.h"

void VerseNode::init()
{
    right = 0;
    left = 0;
    down = 0;
    up = 0;
    parallelId = -1;
}

VerseNode::VerseNode()
{
    init();
}

VerseNode::VerseNode(int parallelId)
{
    init();
    this->parallelId = parallelId;
}

void VerseNode::addTextInfo(TextInfo textInfo)
{
    textInfos.append(textInfo);
}

void VerseNode::insertBelowMe(VerseNode* toInsertNode)
{
    VerseNode* tmp = this->down;

    this->down = toInsertNode;
    toInsertNode->up = this;
    toInsertNode->down = tmp;
    if(tmp != 0)
        tmp->up = toInsertNode;
}

void VerseNode::setRightOfMe(VerseNode* verseNode)
{
    this->right = verseNode;
    verseNode->left = this;
}

VerseNode::~VerseNode()
{
    if(down != 0)
        delete down;
}
