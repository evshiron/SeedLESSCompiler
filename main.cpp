#include "LessParser.h"

#include <iostream>
#include <string>
#include <regex>

using namespace std;

int main() {

    cout << "Hello, World!" << endl;

    //TreeNode* root = new TreeNode();
    //root->type = NodeType::Comment;

    const char *less = R"(

// Comment.
/* Multiline comment. */

@short-color: #4B4;
@nice-blue: #5B83AD;
#test2 {
    color: @nice-blue;
}

#test4 {
  color: black;
  .c1 {
     color: red;
     .c11{
        #test2;
        color:green;
     }
  }
  .c2{
      color:yellow;
  }
}

)";


    const char *less_test3 = R"(

@the-border: 1px;
@base-color: #111111;
@red:
#842210;
@domain:"dian.hust.edu.cn";
#header {
color: @base-color * 6;
border-left: @the-border;
border-right: @the-border * 2;
}
#footer {
color: (@base-color+@red)*2;
background:url("@{domain}/cn/show/slide/1.jpg")
}

)";

    LessParser *parser = new LessParser(less_test3);
    parser->PreParse();
    parser->Parse();

    //cout << rootNode->ToString() << endl;

    return 0;

}