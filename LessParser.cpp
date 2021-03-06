//
// Created by evshiron on 10/19/15.
//

#include "LessParser.h"
#include "ParseNode.h"

#include <iostream>
#include <string>
#include <regex>
#include <stack>
#include <map>

#include "Calculator.h"

#define SIZE_TAB 4

#define FATAL(x) { cerr << x << endl; exit(1); }
#define REGEX_MATCH_COUNT(x) distance(x, sregex_iterator())

using namespace std;

LessParser::LessParser(string less) {

    mInput = string(less);

    mOffset = 0;

    mRootBlock = new BlockNode();
    mRootBlock->Parent = 0;
    mRootBlock->IsRoot = true;
    mRootBlock->Selectors = "";
    mRootBlock->FullSelectors = "";
    mBlockMap[""] = mRootBlock;

    mCurrentBlock = mRootBlock;

}

string LessParser::findVariableValue(BlockNode* blockNode, string key) {

    string value("");

    BlockNode* node = blockNode;

    while(node != 0) {

        ////cerr << node->Selectors << endl;

        auto it = node->Variables.find(key);

        if(it != node->Variables.end()) {

            value = string(it->second);
            goto FIND_VARIABLE_DONE;

        }
        else {

            for(auto jt = node->Children.begin(); jt != node->Children.end(); ++jt) {

                ////cerr << (*jt)->Type << endl;

                if((*jt)->Type == ParseNodeType::Mixin) {

                    BlockNode* linkedBlock = ((MixinNode*) *jt)->LinkedBlock;

                    auto kt = linkedBlock->Variables.find(key);

                    if(kt != linkedBlock->Variables.end()) {

                        value = string(kt->second);
                        goto FIND_VARIABLE_DONE;

                    }

                }

            }

            node = (BlockNode*) node->Parent;
            continue;

        }

    }

    FIND_VARIABLE_DONE:

    if(value == "") FATAL("ERROR_VARIABLE_NOT_FOUND");

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
                goto SKIP_DONE;

        }

    }

    SKIP_DONE:

    return;

}

bool LessParser::tryParseComment() {

    if(mInput[mOffset] == '/' && mInput[mOffset+1] == '*') {

        string input = mInput.substr(mOffset);
        smatch match;
        regex regexComment("^/\\*([\\s\\S]+?)\\*/");

        if(regex_search(input, match, regexComment)) {

            //cerr << "COMMENT: " << match[1] << endl;

            CommentNode* comment = new CommentNode();
            comment->Parent = mCurrentBlock;
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

        string input = mInput.substr(mOffset);
        smatch match;
        regex regexVariable("^@([a-zA-Z-_]+)\\s*:\\s*[\"\']?([\\s\\S]+?)[\"\']?\\s*;");

        if(regex_search(input, match, regexVariable)) {

            //cerr << "VARIABLE: " << match[1] << ": " << match[2] << endl;

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

    string input = mInput.substr(mOffset);
    smatch match;
    // match[1]: match until big bracket.
    // match[2]: selectors.
    // match[3]: arguments.
    regex regexBlockStart("^(([a-zA-Z0-9-_.#]+(?:\\s*,\\s*[a-zA-Z0-9-_.#]+)*)\\s*(?:\\(([\\s\\S]*?)\\))?\\s*\\{)\\s*([^}]+)\\s*\\}");

    if(regex_search(input, match, regexBlockStart)) {

        //cerr << "BLOCK_START: " << match[2] << endl;

        BlockNode* blockNode = new BlockNode();
        blockNode->Parent = mCurrentBlock;
        blockNode->IsRoot = false;
        blockNode->IsFunction = string(match[1]).find("(") == string::npos ? false : true;
        blockNode->Selectors = string(match[2]);

        ////cerr << "BLOCK_START_2: " << match[2] << endl;
        ////cerr << "BLOCK_START_2: " << blockNode->Selectors << endl;

        string fullSelectors;

        BlockNode* node = blockNode;
        fullSelectors.append(node->Selectors);
        while((node = (BlockNode*) node->Parent) != 0) {

            if(node->Selectors != "") fullSelectors.insert(0, " ").insert(0, node->Selectors);

        }

        blockNode->FullSelectors = fullSelectors;

        blockNode->Arguments = string(match[3]);

        mBlockMap[blockNode->FullSelectors] = blockNode;

        mCurrentBlock->Children.push_back(blockNode);
        mCurrentBlock = blockNode;

        mOffset += match[1].length();

        return true;

    }
    else {

        return false;

    }

}

bool LessParser::tryParseMixin() {

    string input = mInput.substr(mOffset);
    smatch match;
    // match[1]: selectors.
    // match[2]: arguments.
    regex regexMixin("^([.#][a-zA-Z0-9-_]+)\\s*(?:\\(([\\s\\S]*?)\\))?\\s*;");

    if(regex_search(input, match, regexMixin)) {

        //cerr << "MIXIN: " << match[1] << "(" << match[2] << ")" << endl;

        MixinNode* mixinNode = new MixinNode();
        mixinNode->Parent = mCurrentBlock;
        mixinNode->Anchor = string(match[1]);
        mixinNode->Arguments = string(match[2]);

        mCurrentBlock->Children.push_back(mixinNode);

        mOffset += match[0].length();

        return true;

    }
    else {

        return false;

    }

}

bool LessParser::tryParseLiteral() {

    string input = mInput.substr(mOffset);
    smatch match;
    regex regexLiteral("^([a-zA-Z-_]+)\\s*:\\s*([^;]+)\\s*;");

    if(regex_search(input, match, regexLiteral)) {

        //cerr << "LITERAL: " << match[1] << ": " << match[2] << endl;

        LiteralNode* literalNode = new LiteralNode();
        literalNode->Parent = mCurrentBlock;
        literalNode->Content = string(match[0]);
        literalNode->Key = string(match[1]);
        literalNode->Value = string(match[2]);

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

        //cerr << "BLOCK_END: " << mCurrentBlock->Selectors << endl;

        mCurrentBlock = (BlockNode*) mCurrentBlock->Parent;

        mOffset++;

        return true;

    }
    else {

        return false;

    }

}

void LessParser::handleMixin(BlockNode* blockNode) {

    //cerr << "MIXIN_START: " << blockNode->FullSelectors << endl;

    for(auto it = blockNode->Children.begin(); it != blockNode->Children.end(); ++it) {

        if((*it)->Type == ParseNodeType::Mixin) {

            MixinNode* mixin = (MixinNode*) *it;

            BlockNode* node = blockNode;
            while((node = (BlockNode*) node->Parent) != 0) {

                for(auto jt = node->Children.begin(); jt != node->Children.end(); ++jt) {

                    if((*jt)->Type == ParseNodeType::Block) {

                        BlockNode* block = (BlockNode*) *jt;

                        if(block->Selectors == mixin->Anchor) {

                            mixin->LinkedBlock = block->Copy();
                            mixin->LinkedBlock->Parent = node;

                            string fullSelectors(mixin->LinkedBlock->Selectors);
                            if(!node->IsRoot) fullSelectors.insert(0, " ").insert(0, node->FullSelectors);
                            mixin->LinkedBlock->FullSelectors = fullSelectors;

                            if(mixin->Arguments != "") mixin->LinkedBlock->FillArguments(mixin->Arguments);

                            goto MIXIN_ONE_DONE;

                        }

                    }

                }

            }

            FATAL("ERROR_MIXIN_LINK_FAILED");

            MIXIN_ONE_DONE:
                //cerr << "MIXIN_LINK: " << mixin->Anchor << " -> " << mixin->LinkedBlock->FullSelectors << endl;
                cout;

        }
        else if((*it)->Type == ParseNodeType::Block) {

            BlockNode* block = (BlockNode*) *it;
            handleMixin(block);

        }

    }

    //cerr << "MIXIN_END: " << blockNode->FullSelectors << endl;

}

void LessParser::handleVariable(BlockNode* blockNode) {

    //cerr << "VARIABLE_START: " << blockNode->FullSelectors << endl;

    blockNode->LoadArguments();

    for(auto it = blockNode->Children.begin(); it != blockNode->Children.end(); ++it) {

        if((*it)->Type == ParseNodeType::Literal) {

            LiteralNode* literal = (LiteralNode*) *it;

            //cerr << "VARIABLE_START_KEY: " << literal->Key << endl;

            string output(literal->Value);

            while(true) {

                // The sregex_iterator saves a pointer to the regex, so you can't use something like begin(a, b, regex()).
                regex regexVariableUse("(?:@([a-zA-Z-_]+)|@\\{([a-zA-Z-_]+)\\})");
                sregex_iterator begin(output.begin(), output.end(), regexVariableUse);
                sregex_iterator end = sregex_iterator();

                //cerr << "Found " << REGEX_MATCH_COUNT(begin) << " variable uses." << endl;

                if(REGEX_MATCH_COUNT(begin) == 0) break;

                for(sregex_iterator it = begin; it != end; ++it) {

                    string key = string((*it)[0]);

                    ////cerr << key << endl;

                    smatch match;

                    regex_search(output, match, regexVariableUse);

                    output.replace(output.find(key), key.length(), findVariableValue(blockNode, string(match[1]) == "" ? match[2] : match[1]));
                    //output = regex_replace(output, regex(key), findVariableValue(blockNode, key.substr(1)));

                }

            }

            literal->Value = output;

        }
        else if((*it)->Type == ParseNodeType::Block) {

            BlockNode* block = (BlockNode*) *it;
            handleVariable(block);

        }
        else if((*it)->Type == ParseNodeType::Mixin) {

            MixinNode* mixin = (MixinNode*) *it;
            handleVariable(mixin->LinkedBlock);

        }

    }

    //cerr << "VARIABLE_END: " << blockNode->FullSelectors << endl;

}

/**
 * gcc  4.9 can't support the regex [\\+\\-\\*\\/],
 * so i write a function to replace it
 */
bool LessParser::needCalculate(string expression)
{
    if(expression.find('+')!=-1)
        return true;
    else if(expression.find('-')!=-1)
        return true;
    else if(expression.find('*')!=-1)
        return true;
    else if(expression.find('/')!=-1)
        return true;
    else
        return false;
}

void LessParser::handleLiteral(BlockNode* blockNode) {

    //cerr << "LITERAL_START: " << blockNode->FullSelectors << endl;

//    regex regexSymbol("[\\+\\-\\*\\/]");
      regex regexRGB("rgb");
      regex regexRGBA("rgba");
      regex regex_color("#[a-fA-F0-9]{6}");
    regex regex_rgb("rgb\\([0-9][^,],[0-9][^,],[0-9][^,]\\)");
    regex regex_rgba("rgba\\([0-9][^,],[0-9][^,],[0-9][^,],[0-9]\\)");
    for(auto it = blockNode->Children.begin(); it != blockNode->Children.end(); ++it) {

        if((*it)->Type == ParseNodeType::Literal) {

            LiteralNode* literal = (LiteralNode*) *it;

            //cerr << "LITERAL_START_KEY: " << literal->Key << endl;

            string output(literal->Value);

            if(needCalculate(output)) {            //todo  add rgb calculate and rgba calculate
//                if(regex_search(output,regexRGB))
                if(output.find("rgb(") != string::npos)
                {
                    output.erase(remove_if(output.begin(), output.end(), ::isspace), output.end());
                    cout << "######################1 start " << output<<endl;
                    sregex_iterator begin(output.begin(),output.end(), regex_rgb);
                    sregex_iterator end = sregex_iterator();
                    for (sregex_iterator it = begin; it != end; ++it) {
                        string key = (*it).str();
                        cout << "######################1 key " << key<<endl;
                        replace(output, key, rgbConvert(key));
                    }
                    cout << "######################1 " <<output << endl;
                }
//                if(regex_search(output,regexRGBA))
                if(output.find("rgba(") != string::npos)
                {
                    output.erase(remove_if(output.begin(), output.end(), ::isspace), output.end());
                    cout << "######################2 start "<<output << endl;
                    sregex_iterator begin(output.begin(),output.end(), regex_rgba);
                    sregex_iterator end = sregex_iterator();
                    for (sregex_iterator it = begin; it != end; ++it) {
                        string key = (*it).str();
                        replace(output, key, rgbaConvert(key));
                    }
                    cout << "######################2 " <<output << endl;
                }
                //cerr << "LITERAL_START_VALUE: " << literal->Value << endl;
                output = Calculate(output);

            }

            literal->Value = output;

        }
        else if((*it)->Type == ParseNodeType::Block) {

            BlockNode* block = (BlockNode*) *it;
            handleLiteral(block);

        }
        else if((*it)->Type == ParseNodeType::Mixin) {

            MixinNode* mixin = (MixinNode*) *it;
            handleLiteral(mixin->LinkedBlock);

        }



    }

    //cerr << "LITERAL_END: " << blockNode->FullSelectors << endl;

}

string LessParser::outputBlock(BlockNode* blockNode, int indent) {

    if(blockNode->IsFunction) {

        //cerr << "FUNCTION_BLOCK: " << blockNode->FullSelectors << endl;

        return "";

    }

    //cerr << "BLOCK: " << blockNode->FullSelectors << endl;

    list<ParseNode*> pendingNodes;
    list<BlockNode*> pendingBlocks;

    BlockNode* node = blockNode;

    for(auto it = node->Children.begin(); it != node->Children.end(); ++it) {

        BlockNode* linkedBlock = 0;

        switch((*it)->Type) {

            case ParseNodeType::Comment:
            case ParseNodeType::Literal:
                pendingNodes.push_back(*it);
                break;
            case ParseNodeType::Mixin:

                linkedBlock = ((MixinNode*) *it)->LinkedBlock;

                for(auto jt = linkedBlock->Children.begin(); jt != linkedBlock->Children.end(); ++jt) {

                    switch((*jt)->Type) {

                        case ParseNodeType::Literal:
                            pendingNodes.push_back(*jt);
                            break;
                        case ParseNodeType::Mixin:
                            FATAL("ERROR_MIXIN_IN_MIXIN");

                    }

                }

                break;
            case ParseNodeType::Block:
                pendingBlocks.push_back((BlockNode*) *it);
                break;

        }

    }

    string output;

    if(pendingNodes.size() > 0) {

        if(!blockNode->IsRoot) output.append(string(indent, ' ')).append(blockNode->FullSelectors).append(" {\n");

        for(auto it = pendingNodes.begin(); it != pendingNodes.end(); ++it) {

            output.append(string(indent+SIZE_TAB, ' ')).append((*it)->ToString());

        }

        if(!blockNode->IsRoot) output.append(string(indent, ' ')).append("}\n");

    }

    for(auto it = pendingBlocks.begin(); it != pendingBlocks.end(); ++it) {

        output.append(outputBlock(*it, indent));

    }

    return output;

}

void LessParser::PreParse() {

    string output(mInput);
    //cerr << output << endl;
    // string regex_replace(subject, regex, replacement).
    // /r, /r/n -> /n.
    output = regex_replace(output, regex("\r\n?"), "\n");
    // Strip out single comments.
    output = regex_replace(output, regex("//[^\n]+"), "");
    // Expand colors in short form.
    //output = regex_replace(output, regex("#([a-zA-Z0-9])([a-zA-Z0-9])([a-zA-Z0-9])([^a-zA-Z0-9])"), "#$1$1$2$2$3$3$4");
    output = regex_replace(output, regex("#([a-fA-F0-9])([a-fA-F0-9])([a-fA-F0-9])([^a-fA-F0-9])"), "#$1$1$2$2$3$3$4");

    //cerr << "PreParse: " << output << endl;

    mInput = output;

}

void LessParser::Parse() {

    ////cerr << mInput.length() << endl;

    int lastOffset = 0;

    while(true) {

        skipW(0);

        ////cerr << mInput[mOffset] << endl;

        if(tryParseComment()) continue;
        if(tryParseVariable()) continue;
        if(tryParseBlockStart()) continue;
        if(tryParseMixin()) continue;
        if(tryParseLiteral()) continue;
        if(tryParseBlockEnd()) continue;

        if(mOffset == lastOffset) {

            if(mOffset >= mInput.length() - 1) {

                //cerr << "EOF" << endl;
                break;

            }
            else {

                //cerr << "SKIP: " << mInput[mOffset] << " @ " << mOffset << endl;
                mOffset++;

            }

        }

        lastOffset = mOffset;

    }

}

void LessParser::Handle() {

    handleMixin(mRootBlock);
    handleVariable(mRootBlock);
    handleLiteral(mRootBlock);

}

string LessParser::GetCSS() {

    return outputBlock(mRootBlock, 0);

}

string rgbConvert(string rgb)
{
    rgb.erase(remove_if(rgb.begin(), rgb.end(), ::isspace), rgb.end());
    regex regexNumber("[0-9][^,]");
    sregex_iterator begin(rgb.begin(),rgb.end(), regexNumber);
    sregex_iterator end = sregex_iterator();
    int count=0;
    int r,g,b;
    for (sregex_iterator it = begin; it != end; ++it) {
        string key = (*it).str();
        cout << "######regex number "<<count << key<<endl;
        if(count==0)
            r=stoi(key);
        else if(count==1)
            g=stoi(key);
        else if(count==2)
            b=stoi(key);
        else{}
        count++;
    }
    cout<<"rbg"<<r<<g<<b<< endl;
    return rgbToString(r,g,b);
}

string rgbaConvert(string rgba)
{
    rgba.erase(remove_if(rgba.begin(), rgba.end(), ::isspace), rgba.end());
    regex regexNumber("[0-9][^,]");
    sregex_iterator begin(rgba.begin(),rgba.end(), regexNumber);
    sregex_iterator end = sregex_iterator();
    int count=0;
    int r,g,b;
    for (sregex_iterator it = begin; it != end; ++it) {
        string key = (*it).str();
        if(count==0)
            r=stoi(key);
        else if(count==1)
            g=stoi(key);
        else if(count==2)
            b=stoi(key);
        else{}
        count++;
    }
    cout<<"rbg"<<r<<g<<b<< endl;
    return rgbToString(r,g,b);
}


