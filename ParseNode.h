//
// Created by evshiron on 10/20/15.
//

#ifndef SEEDLESSCOMPILER_PARSENODE_H
#define SEEDLESSCOMPILER_PARSENODE_H

#include <iostream>
#include <list>
#include <string>
#include <map>
#include <regex>

#define FATAL(x) { cout << x << endl; exit(1); }

using namespace std;

enum ParseNodeType {

    Unknown, Block, Comment, Mixin, Literal

};

class ParseNode {

private:

public:

    ParseNodeType Type = Unknown;
    ParseNode* Parent = 0;
    list<ParseNode*> Children;

    virtual ParseNode* Copy() {

        return new ParseNode();

    }

    virtual string ToString() {

        return string();

    }

};

class BlockNode : public ParseNode {

public:

    bool IsRoot;
    bool IsFunction;
    string Selectors;
    string FullSelectors;
    string Arguments;
    map<string, string> Variables;

    BlockNode() {

        Type = ParseNodeType::Block;
        IsRoot = false;
        IsFunction = false;

    }

    void FillArguments(string arguments);
    void LoadArguments();

    BlockNode* Copy() {

        BlockNode* blockNode = new BlockNode();

        blockNode->IsRoot = IsRoot;
        blockNode->Selectors = string(Selectors);
        blockNode->FullSelectors = string(FullSelectors);
        blockNode->Arguments = string(Arguments);

        auto it = Variables.begin();
        while(it != Variables.end()) {

            blockNode->Variables[string(it->first)] = string(it->second);
            ++it;

        }

        auto jt = Children.begin();
        while(jt != Children.end()) {

            // Strip out MIXINs avoiding loops.
            if((*jt)->Type != ParseNodeType::Mixin) blockNode->Children.push_back((*jt)->Copy());
            ++jt;

        }

        return blockNode;

    }

    string ToString() {

        string str;

        if(!IsRoot) {

            string selectors(Selectors);
            selectors.append(" ");

            //cout << selectors << endl;

            str.append(selectors);
            str.append("{\n");

        }

        for(list<ParseNode*>::iterator it = Children.begin(); it != Children.end(); ++it) {

            ParseNode* node = *it;

            str.append(node->ToString()).append("\n");

        }


        if(!IsRoot) str.append("}\n");

        return str;

    }

};

class CommentNode : public ParseNode {

public:

    string Content;

    CommentNode() {

        Type = ParseNodeType::Comment;

    }

    CommentNode* Copy() {

        CommentNode* commentNode = new CommentNode();
        commentNode->Content = Content;

        return commentNode;

    }

    string ToString() {

        string str;

        str.append("/*").append(Content).append("*/\n");

        return str;

    }

};

class MixinNode : public ParseNode {

public:

    string Anchor;
    string Arguments;
    BlockNode* LinkedBlock = 0;

    MixinNode() {

        Type = ParseNodeType::Mixin;
        LinkedBlock = 0;

    }

    MixinNode* Copy() {

        MixinNode* mixinNode = new MixinNode();
        mixinNode->Anchor = string(Anchor);
        mixinNode->Arguments = string(Arguments);
        mixinNode->LinkedBlock = LinkedBlock->Copy();

        return mixinNode;

    }

    string ToString() {

        string str;

        str.append(Anchor).append(";\n");

        return str;

    }

};

class LiteralNode : public ParseNode {

public:

    string Content;
    string Key;
    string Value;

    LiteralNode() {

        Type = ParseNodeType::Literal;

    }

    LiteralNode* Copy() {

        LiteralNode* literalNode = new LiteralNode();
        literalNode->Content = string(Content);
        literalNode->Key = string(Key);
        literalNode->Value = string(Value);

        return literalNode;

    }

    string ToString() {

        string str;

        str.append(Key).append(": ").append(Value).append(";\n");

        return str;

    }

};

#endif //SEEDLESSCOMPILER_PARSENODE_H
