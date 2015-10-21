//
// Created by evshiron on 10/19/15.
//

#ifndef SEEDLESSCOMPILER_LESSPARSER_H
#define SEEDLESSCOMPILER_LESSPARSER_H

#include "ParseNode.h"

class LessParser {

private:

    string mInput;
    int mOffset = 0;

    map<string, BlockNode*> mBlockMap;
    list<BlockNode*> mBlockList;

    BlockNode* mRootBlock = 0;
    BlockNode* mCurrentBlock = 0;


public:

    LessParser(string less);

    void skipW(int count);
    bool tryParseComment();
    bool tryParseVariable();
    bool tryParseBlockStart();
    bool tryParseLiteral();
    bool tryParseBlockEnd();

    void PreParse();
    void Parse();

};


#endif //SEEDLESSCOMPILER_LESSPARSER_H
