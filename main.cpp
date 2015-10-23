#include "LessParser.h"

#include <iostream>
#include <string>
#include <regex>

using namespace std;

int main() {

    cout << "Hello, World!" << endl;

    //TreeNode* root = new TreeNode();
    //root->type = NodeType::Comment;

    const char* less = R"(

// Comment.
/* Multiline comment. */

@short-color: #4B4; // Comment.
@nice-blue: #5B83AD;
#test2 {
    color: @nice-blue;
    @green: #00ff00;
    @url: "aaa";
}

#test4 {
  color: black;
  .c1 {
     color: red;
     .c11{
        #test2;
        color:@green;
        background-image: url("@{url}");
     }
  }
  .c2{
      color:yellow;
  }
}

/*

Output:

#test2 {
    color: #5B83AD;
}

#test4 {
    color: black;
    .c1 {
        color: red;
        .c11 {
            #test2;
            color:green;
        }
    }
    .c2 {
        color:yellow;
    }
}

*/

)";

    LessParser* parser = new LessParser(less);
    parser->PreParse();
    parser->Parse();
    parser->Handle();

    cout << "Output: " << parser->GetCSS() << endl;

    return 0;

}