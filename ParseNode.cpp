//
// Created by evshiron on 10/20/15.
//

#include "ParseNode.h"

void BlockNode::FillArguments(string arguments) {

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

void BlockNode::LoadArguments() {

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

