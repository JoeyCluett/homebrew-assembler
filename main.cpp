#include <iostream>
#include <stdio.h>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/tokenerror.h"
#include "inc/parser.h"
#include "inc/parsererror.h"

using namespace std;

std::string ljust(std::string s, int length);

int main(int argc, char* argv[]) {

    //std::string filename = "examples/helloworld.txt";
    //std::string filename = "examples/memoryops.txt";
    std::string filename = "examples/multiword.txt";


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
            generate_parser_error(vchar, pe.offset, pe.message, filename);
            return 1;
        }
    } catch(TokenizeException& te) {
        generate_tokenizer_error(vchar, te.offset, te.message, filename);
        return 1;
    }

    return 0;
}

std::string ljust(std::string s, int length) {
    while(s.size() < length)
        s.push_back(' ');
    return s;
}

