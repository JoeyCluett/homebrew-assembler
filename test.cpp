#include <iostream>
#include <string>
#include <vector>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/parser.h"
#include "inc/assemblererror.h"

using namespace std;

struct arguments_t {
    struct {
        string filename; // optional, may also just pass code directly on command line
        int expected_result;
    } opts;

    vector<char> code;
    string program_name;
};

arguments_t parse_input_arguments(vector<string>& v);

int call_assembler(std::string filename, std::vector<char>& vchar);

int main(int argc, char* argv[]) {

    // string are more convenient than const char*s
    std::vector<std::string> arg_strings;
    for(int i = 0; i < argc; i++) {
        arg_strings.push_back(argv[i]);
    }

    arguments_t args = parse_input_arguments(arg_strings);

    if(argc != 3) {
        cout << "usage:\n  " << argv[0] << " <file_to_test> <expected_result>\n";
        return 1;
    }

    int returned_value = call_assembler(args.opts.filename, args.code);

    cout << returned_value;

    if(args.opts.expected_result == returned_value)
        return 0;
    return 1;
}

arguments_t parse_input_arguments(vector<string>& v) {

    arguments_t args;

    const int state_program_name = 0;
    const int state_default      = 1;
    const int state_inline_code  = 2;
    const int state_filename     = 3;
    int state_current = state_program_name;


    bool code_inline = false;
    bool code_in_file = false;

    for(auto& s : v) {
        switch(state_current) {
            case state_program_name:
                args.program_name = s;
                state_current = state_default;
            case state_default:
                if(s == "-c") { // code
                    state_current = state_inline_code;
                }
                else if(s == "-e") { // external file
                    state_current = 
                }
                break;
            case state_inline_code:

            case state_filename:


        }
    }

}

int call_assembler(std::string filename, std::vector<char>& vchar) {

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

