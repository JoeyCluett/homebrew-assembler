#include <iostream>
#include <fstream>
#include <stdio.h>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/parser.h"
#include "inc/assemblererror.h"

using namespace std;

std::string ljust(std::string s, int length);
void write_error_to_file(int i);

int main(int argc, char* argv[]) {

    std::string filename = argv[1];
    int print_exception  = std::stoi(argv[2]);

    auto vchar  = read_file(filename);
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
                if(print_exception)
                    generate_assembler_error("ParseException", vchar, pe.offset, pe.message, filename);
                write_error_to_file(2);
                return 2;
            }
        } catch(TokenizeException& te) {
            if(print_exception)
                generate_assembler_error("TokenizeException", vchar, te.offset, te.message, filename);
            write_error_to_file(3);
            return 3;
        }
    } catch(std::exception& ex) {
        if(print_exception)
            cerr << "test generated std::runtime_error" << endl;
        write_error_to_file(1);
        return 1;
    }

    write_error_to_file(0);
    return 0;
}

void write_error_to_file(int i) {
    std::ofstream ofile("assembly_error_output.txt", std::ios_base::out);
    ofile << i << std::endl << std::flush;
    ofile.close();
}

std::string ljust(std::string s, int length) {
    while(s.size() < length)
        s.push_back(' ');
    return s;
}

