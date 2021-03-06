#include "parallelgridconstructor.h"

#include <QList>
#include <QDebug>

//VerseNode* ParallelGridConstructor::constructChain(QList<TextInfo> textInfos)
//{
//    if(textInfos.size() > 0)
//    {
//        VerseNode* result = new VerseNode();
//        VerseNode* current = new VerseNode(textInfos.at(0).parallel);;
//        result->insertBelowMe(current);

//        TextInfo value;
//        foreach (value, textInfos)
//        {
//            if(value.parallel == current->parallelId)
//            {
//                current->addTextInfo(value);
//            }
//            else
//            {
//                VerseNode* nextVerseNode = new VerseNode(value.parallel);
//                nextVerseNode->addTextInfo(value);
//                current->insertBelowMe(nextVerseNode);
//                current = nextVerseNode;
//            }
//        }
//        return result;
//    }

//    return new VerseNode();
//}

Grid ParallelGridConstructor::constructGrid(QList<VerseNode*> chainHeads)
{
    for(int i=0; i<chainHeads.size()-1; i++)
    {
        VerseNode* chainHead = chainHeads.at(i);
        chainHead->setRightOfMe(chainHeads.at(i+1));
    }

    for(int i=0; i<chainHeads.size()-1; i++)
    {
        VerseNode* chainHead = chainHeads.at(i);
        VerseNode* nextChainHead = chainHeads.at(i+1);
        VerseNode* nextChainTail = nextChainHead;
        while(nextChainTail->down != 0)
        {
            nextChainTail = nextChainTail->down;
        }

        VerseNode* node = chainHead->down;
        while(node != 0)
        {
            VerseNode* nextChainNode = nextChainHead->down;
            while(nextChainNode != 0)
            {
                if(nextChainNode->parallelId == node->parallelId)
                {
                    node->setRightOfMe(nextChainNode);
                    break;
                }
                nextChainNode = nextChainNode->down;
            }
            if(node->right == 0)
            {
                VerseNode* toInsertNode = new VerseNode(node->parallelId);
                nextChainTail->insertBelowMe(toInsertNode);
                node->setRightOfMe(toInsertNode);
            }
            node = node->down;
        }

        VerseNode* nextChainNode = nextChainHead->down;
        while(nextChainNode != 0)
        {
            if(nextChainNode->left == 0)
            {
                VerseNode* target = nextChainNode;
                VerseNode* currentLeft = target->up->left;
                while(currentLeft != 0)
                {
                    VerseNode* toInsertNode = new VerseNode(nextChainNode->parallelId);
                    currentLeft->insertBelowMe(toInsertNode);
                    toInsertNode->setRightOfMe(target);

                    target = toInsertNode;
                    currentLeft = currentLeft->left;
                }

            }
            nextChainNode = nextChainNode->down;
        }

    }

    bool evenNumberOfRows = true;
    VerseNode* count = chainHeads.at(0);
    while(count->down != 0)
    {
        evenNumberOfRows = !evenNumberOfRows;
        count = count->down;
    }

    return Grid(chainHeads.at(0),evenNumberOfRows);

}
