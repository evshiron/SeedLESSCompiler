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

    void FillArguments(string arguments) {

        string output(Arguments);

        //cout << "arguments: " << arguments << endl;
        //cout << "Arguments: " << Arguments << endl;

        // For mixin arguments.
        regex regexArgumentDelimiter("\\s*[,]\\s*");
        sregex_token_iterator mbegin(arguments.begin(), arguments.end(), regexArgumentDelimiter, -1);
        sregex_token_iterator mend = sregex_token_iterator();
        // For block arguments.
        regex regexArgumentDefine("@([a-zA-Z-_]+)\\s*:\\s*[\"\']?([^,;]+)[\"\']?\\s*");
        sregex_iterator bbegin(Arguments.begin(), Arguments.end(), regexArgumentDefine);
        sregex_iterator bend = sregex_iterator();

        if(distance(mbegin, mend) > distance(bbegin, bend)) FATAL("ERROR_MIXIN_ARGUMENT");

        sregex_token_iterator it = mbegin;
        sregex_iterator jt = bbegin;
        for(; it != mend; ++it, ++jt) {

            string key((*jt)[1]);
            string oldValue((*jt)[2]);
            string newValue((*it));
            //cout << key << ": " << oldValue << " -> " << newValue << endl;

            output.replace(output.find(oldValue), oldValue.length(), newValue);

        }

        Arguments = string(output);

    }

    void LoadArguments() {

        map<string, string> argumentStore;

        regex regexArgumentDefine("@([a-zA-Z-_]+)\\s*:\\s*[\"\']?([^,;]+)[\"\']?\\s*");
        sregex_iterator begin(Arguments.begin(), Arguments.end(), regexArgumentDefine);
        sregex_iterator end = sregex_iterator();

        cout << "Found " << distance(begin, end) << " arguments." << endl;

        for(sregex_iterator it = begin; it != end; ++it) {

            cout << (*it)[1] << ": " << (*it)[2] << endl;
            argumentStore[string((*it)[1])] = string((*it)[2]);

        }

        for(auto it = argumentStore.begin(); it != argumentStore.end(); ++it) {

            if(Variables.count(it->first) == 0) {

                Variables[string(it->first)] = string(argumentStore[it->first]);

            }

        }

        cout << "Arguments loaded." << endl;

    }

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
