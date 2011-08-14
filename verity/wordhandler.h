#ifndef WORDHANDLER_H
#define WORDHANDLER_H

class WordHandler
{
public:
    WordHandler();
    virtual void handle(int bibletextId, int wordId) = 0;
};

#endif // WORDHANDLER_H
