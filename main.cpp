#include "LessParser.h"

#include <iostream>
#include <string>
#include <regex>

using namespace std;

#define FATAL(x) { cerr << x << endl; exit(1); }

int main(int argc, const char** argv) {

    if(argc < 2) {

        cerr << R"(Usage: SeedLESSCompiler <source.less> > <dest.css>)" << endl;

    }
    else {

        FILE* file = fopen(argv[1], "rb");
        fseek(file, 0L, SEEK_END);
        size_t size = ftell(file);
        rewind(file);

        char bytes[size];

        size_t s = fread(bytes, 1, size, file);
        if(size != s) FATAL("ERROR_SIZE_NOT_MATCH");

        LessParser* parser = new LessParser(bytes);
        parser->PreParse();
        parser->Parse();
        parser->Handle();

        cout << parser->GetCSS() << endl;

        return 0;

    }


}