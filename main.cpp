#include <iostream>
#include <stdio.h>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/parser.h"
#include "inc/assemblererror.h"

using namespace std;

std::string ljust(std::string s, int length);

int main(int argc, char* argv[]) {

    //std::string filename = "examples/helloworld.txt";
    //std::string filename = "examples/memoryops.txt";
    //std::string filename = "examples/multiword.txt";

    std::string filename;

    if(argc == 2) {
        filename = argv[1];
    }
    else {
        filename = "examples/multiword.txt";
    }

    auto vchar  = read_file(filename);
    //cout.write(vchar.data(), vchar.size());

    auto iter = vchar.begin();

    GeneratedIR gir;

    try {
        try {

            auto token = get_token(vchar, iter);
            while(token.valid_token && iter != vchar.end()) {
                
                //cout << ljust(token.typestr(), 15) << token.str(vchar) << endl;
                //cout << token.idxstart << " - " << token.idxend << endl;

                // process token...
                consume_token(gir, vchar, token);

                // get next token
                token = get_token(vchar, iter);
            }

        } catch(ParseException& pe) {
            generate_assembler_error("ParseException", vchar, pe.offset, pe.message, filename);
            return 1;
        }
    } catch(TokenizeException& te) {
        generate_assembler_error("TokenizeException", vchar, te.offset, te.message, filename);
        return 1;
    }

    cout << "number of instructions: " << gir.ir.size() << endl;

    return 0;
}

std::string ljust(std::string s, int length) {
    while(s.size() < length)
        s.push_back(' ');
    return s;
}

