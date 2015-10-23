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

    // Map of Pair<FullSelectors, BlockNode*>.
    map<string, BlockNode*> mBlockMap;

    BlockNode* mRootBlock = 0;
    BlockNode* mCurrentBlock = 0;


public:

    LessParser(string less);

    BlockNode* GetRootBlock() {

        return mRootBlock;

    }

    string findVariableValue(BlockNode* blockNode, string key);
    void skipW(int count);

    bool tryParseComment();
    bool tryParseVariable();
    bool tryParseBlockStart();
    bool tryParseMixin();
    bool tryParseLiteral();
    bool tryParseBlockEnd();

    void handleMixin(BlockNode* blockNode);
    void handleVariable(BlockNode* blockNode);
    void handleLiteral(BlockNode* blockNode);

    string outputBlock(BlockNode* blockNode, int indent);

    void PreParse();
    void Parse();
    void Handle();
    string GetCSS();

};


#endif //SEEDLESSCOMPILER_LESSPARSER_H
