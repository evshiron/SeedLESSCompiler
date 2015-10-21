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

    // Used in mixin.
    map<string, BlockNode*> mBlockMap;
    // Want to use in flatten.
    //list<BlockNode*> mBlockList;

    BlockNode* mRootBlock = 0;
    BlockNode* mCurrentBlock = 0;


public:

    LessParser(string less);

    string findVariableValue(string key);
    void skipW(int count);
    bool tryParseComment();
    bool tryParseVariable();
    bool tryParseBlockStart();
    bool tryParseMixin();
    bool tryParseLiteral();
    bool tryParseBlockEnd();

    void PreParse();
    void Parse();

};


#endif //SEEDLESSCOMPILER_LESSPARSER_H
