//
// Created by evshiron on 10/19/15.
//

#include "LessParser.h"

#include <iostream>
#include <string>
#include <regex>
#include <stack>
#include <map>

#define FATAL(x) { cout << x << endl; exit(1); }
#define REGEX_MATCH_COUNT(x) distance(x, sregex_iterator())

using namespace std;

LessParser::LessParser(string less) {

    mInput = string(less);

    mOffset = 0;

    mRootBlock = new BlockNode();
    mRootBlock->IsRoot = true;

    mBlockMap["#ROOT#"] = mRootBlock;

    mCurrentBlock = mRootBlock;

}

string LessParser::findVariableValue(string key) {

    string value("");

    BlockNode* node = mCurrentBlock;

    while(node != 0) {

        //cout << node->Selectors << endl;

        auto it = node->Variables.find(key);

        if(it != node->Variables.end()) {

            value = string(it->second);
            break;

        }
        else {

            node = (BlockNode*) node->Parent;
            continue;

        }

    }

    return value;

}

// Skip an amount of characters and find next token (skip whitespaces).
void LessParser::skipW(int count) {

    mOffset += count;

    while(true) {

        switch(mInput[mOffset]) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                mOffset++;
                break;
            default:
                goto OUT_OF_LOOP;

        }

    }

    OUT_OF_LOOP:

    return;

}

bool LessParser::tryParseComment() {

    if(mInput[mOffset] == '/' && mInput[mOffset+1] == '*') {

        smatch match;

        if(regex_search(mInput.substr(mOffset), match, regex("^/\\*\\s*(.+?)\\s*\\*/"))) {

            cout << "COMMENT: " << match[1] << endl;

            CommentNode* comment = new CommentNode();
            comment->Content = string(match[1]);

            mCurrentBlock->Children.push_back(comment);

            mOffset += match[0].length();

            return true;

        }
        else {

            FATAL("ERROR_PARSE_COMMENT");

        }

    }
    else {

        return false;

    }

}

bool LessParser::tryParseVariable() {

    if(mInput[mOffset] == '@') {

        smatch match;

        if(regex_search(mInput.substr(mOffset), match, regex("^@([a-zA-Z-_]+):\\s*([^;]+)\\s*;"))) {

            cout << "VARIABLE: " << match[1] << ": " << match[2] << endl;

            mCurrentBlock->Variables[string(match[1])] = string(match[2]);

            mOffset += match[0].length();

            return true;

        }
        else {

            FATAL("ERROR_PARSE_VARIABLE");

        }

    }
    else {

        return false;

    }

}

bool LessParser::tryParseBlockStart() {

    smatch match;

    if(regex_search(mInput.substr(mOffset), match, regex("^(([a-zA-Z0-9-_.#]+(?:\\s*,\\s*[a-zA-Z0-9-_.#]+)*)\\s*\\{)\\s*([^}]+)\\s*\\}"))) {

        cout << "BLOCK_START: " << match[2] << endl;

        // FIXME: WTF new operation hurts match[2]?!!!
        string selectors(match[2]);
        BlockNode* blockNode = new BlockNode();
        blockNode->Parent = mCurrentBlock;
        //blockNode->Selectors = string(match[2]);
        blockNode->Selectors = selectors;

        //cout << "BLOCK_START_2: " << match[2] << endl;
        //cout << "BLOCK_START_2: " << blockNode->Selectors << endl;

        mCurrentBlock->Children.push_back(blockNode);
        mCurrentBlock = blockNode;

        mBlockMap[selectors] = blockNode;

        mOffset += match[1].length();

        return true;

    }
    else {

        return false;

    }

}

bool LessParser::tryParseMixin() {

    smatch match;

    if(regex_search(mInput.substr(mOffset), match, regex("^([.#][a-zA-Z0-9-_]+);"))) {

        cout << "MIXIN: " << match[1] << endl;

        MixinNode* mixinNode = new MixinNode();
        mixinNode->Parent = mCurrentBlock;
        mixinNode->Anchor = string(match[1]);

        mCurrentBlock->Children.push_back(mixinNode);

        mOffset += match[0].length();

        return true;

    }
    else {

        return false;

    }

}

bool LessParser::tryParseLiteral() {

    smatch match;

    if(regex_search(mInput.substr(mOffset), match, regex("^([a-zA-Z-_]+):\\s*([^;]+);"))) {

        cout << "LITERAL: " << match[1] << ": " << match[2] << endl;

        LiteralNode* literalNode = new LiteralNode();
        literalNode->Parent = mCurrentBlock;
        literalNode->Content = string(match[0]);

        mCurrentBlock->Children.push_back(literalNode);

        mOffset += match[0].length();

        return true;

    }
    else {

        return false;

    }

}

bool LessParser::tryParseBlockEnd() {

    if(mInput[mOffset] == '}') {

        cout << "BLOCK_END: " << mCurrentBlock->Selectors << endl;

        for(auto it = mCurrentBlock->Children.begin(); it != mCurrentBlock->Children.end(); ++it) {

            //cout << ((ParseNode*) *it)->Type << endl;

            switch(((ParseNode*) *it)->Type) {
                case ParseNodeType::Mixin:

                    // -------- Begin Handling Mixin --------
                    // TODO:
                    // -------- End Handing Mixin --------

                    break;

                case ParseNodeType::Literal:

                    LiteralNode* literalNode = (LiteralNode*) *it;

                    string output(literalNode->Content);

                    // -------- Begin Handing Variables --------

                    // The sregex_iterator saves a pointer to the regex, so you can't use something like begin(a, b, regex()).
                    regex regexVariableUse("@([a-zA-Z-_]+)");
                    sregex_iterator begin(output.begin(), output.end(), regexVariableUse);
                    sregex_iterator end = sregex_iterator();

                    cout << "Found " << REGEX_MATCH_COUNT(begin) << " variable uses." << endl;

                    for(sregex_iterator it = begin; it != end; ++it) {

                        string key = (*it).str();

                        //cout << key << endl;

                        output = regex_replace(output, regex(key), findVariableValue(key.substr(1)));

                    }

                    // -------- End Handing Variables --------

                    literalNode->Content = output;

                    break;
            }

        }

        mCurrentBlock = (BlockNode*) mCurrentBlock->Parent;

        mOffset++;

        return true;

    }
    else {

        return false;

    }

}

void LessParser::PreParse() {

    string output(mInput);
    cout << output << endl;
    // string regex_replace(subject, regex, replacement).
    // /r, /r/n -> /n.
    output = regex_replace(output, regex("\r\n?"), "\n");
    // Strip out single comments.
    output = regex_replace(output, regex("//[^\n]+"), "");
    // Expand colors in short form.
    //output = regex_replace(output, regex("#([a-zA-Z0-9])([a-zA-Z0-9])([a-zA-Z0-9])([^a-zA-Z0-9])"), "#$1$1$2$2$3$3$4");
    output = regex_replace(output, regex("#([a-fA-F0-9])([a-fA-F0-9])([a-fA-F0-9])([^a-fA-F0-9])"), "#$1$1$2$2$3$3$4");

    cout << "PreParse: " << output << endl;

    mInput = output;

}

void LessParser::Parse() {

    cout << mInput.length() << endl;

    int lastOffset = 0;

    while(true) {

        skipW(0);

        //cout << mInput[mOffset] << endl;

        if(tryParseComment()) continue;
        if(tryParseVariable()) continue;
        if(tryParseBlockStart()) continue;
        if(tryParseMixin()) continue;
        if(tryParseLiteral()) continue;
        if(tryParseBlockEnd()) continue;

        if(mOffset == lastOffset) {

            if(mOffset >= mInput.length() - 1) {

                cout << "EOF" << endl;
                break;

            }
            else {

                cout << "SKIP: " << mInput[mOffset] << " @ " << mOffset << endl;
                mOffset++;

            }

        }

        lastOffset = mOffset;

    }

}
