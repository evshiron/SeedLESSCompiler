//
// Created by evshiron on 10/20/15.
//

#ifndef SEEDLESSCOMPILER_PARSENODE_H
#define SEEDLESSCOMPILER_PARSENODE_H

#include <iostream>
#include <list>
#include <string>
#include <map>

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

    virtual string ToString() {

        return string();

    };

};

class BlockNode : public ParseNode {

public:

    bool IsRoot;
    string Selectors;
    string FullSelectors;
    string Arguments;
    map<string, string> Variables;

    BlockNode() {

        Type = ParseNodeType::Block;
        IsRoot = false;

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

    string ToString() {

        string str;

        str.append(Key).append(": ").append(Value).append(";\n");

        return str;

    }

};

#endif //SEEDLESSCOMPILER_PARSENODE_H
