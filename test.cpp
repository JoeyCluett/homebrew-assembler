#include <iostream>
#include <string>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/parser.h"
#include "inc/assemblererror.h"

using namespace std;

int call_assembler(std::string filename);

int main(int argc, char* argv[]) {

    if(argc != 3) {
        cout << "usage:\n  " << argv[0] << " <file_to_test> <expected_result>\n";
        return 1;
    }

    std::string filename = argv[1];
    int expecting_value = std::stol(argv[2]);
    int returned_value = call_assembler(filename);

    cout << returned_value;

    if(expecting_value == returned_value)
        return 0;
    return 1;
}

int call_assembler(std::string filename) {

    auto vchar = read_file(filename);
    auto iter = vchar.begin();

    GeneratedIR gir;

    try {
        try {
            try {

                auto token = get_token(vchar, iter);
                while(token.valid_token && iter != vchar.end()) {
                    consume_token(gir, vchar, token);
                    token = get_token(vchar, iter);
                }

            } catch(ParseException& pe) {
                return 2;
            }
        } catch(TokenizeException& te) {
            return 3;
        }
    } catch(std::exception& ex) {
        cerr << "test generated std::runtime_error" << endl;
        return 1;
    }

    return 0;
}

