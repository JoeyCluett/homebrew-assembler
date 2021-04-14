#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <assert.h>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/parser.h"
#include "inc/assemblererror.h"

using namespace std;

std::string RED = "\033[38;2;255;0;0m";
std::string GRN = "\033[38;2;0;255;0m";
std::string BLU = "\033[38;2;0;0;255m";
std::string YEL = "\033[38;2;255;255;0m";
std::string MAG = "\033[38;2;255;0;255m";
std::string CYN = "\033[38;2;0;255;255m";
std::string RST = "\033[0;00m";

int call_assembler(std::string filename, std::vector<char>& vchar, bool print_on_exception);
std::string strip_line(std::string s);
std::vector<std::string> split_line(std::string s);
std::string to_upper(std::string s);
int read_error_code_file(void);
void print_testing_results(int passed, int failed, int total);

int main(int argc, char* argv[]) {

    if(argc < 2) {
        cout << "\nusage:\n  " << CYN << argv[0] << YEL << " <file_to_test>\n" << RST;
        cout << "\noptions:\n";
        cout << CYN << "   --exceptfail     " << RST << ": cause testing program to exit on first failed test\n";
        cout << CYN << "   --printexcept    " << RST << ": print exception data if/when thrown\n";
        cout << CYN << "   --printtest      " << RST << ": print instruction being tested\n\n";
        return 1;
    }

    int fail_on_exception      = 0;
    int print_exception_data   = 0;
    int print_test_instruction = 0;

    int total_failed_tests = 0;
    int total_passed_tests = 0;
    int total_tests = 0;

    std::ifstream input_stream(argv[1], std::ios_base::in);
    std::string line_contents;

    for(int i = 2; i < argc; i++) {
        std::string arg = argv[i]; 
        if(arg == "--exceptfail")  fail_on_exception = 1;
        if(arg == "--printexcept") print_exception_data = 1;
        if(arg == "--printtest")   print_test_instruction = 1;  
    }

    std::vector<std::string> error_type_lut = {
        "ErrNone",
        "ErrExcept",
        "ErrParse",
        "ErrToken"
    };

    while(std::getline(input_stream, line_contents)) {
        auto s = strip_line(line_contents);

        if(line_contents != "#STARTTEST")
            continue;

        std::getline(input_stream, line_contents);
        auto chunks = split_line(strip_line(line_contents));

        int n_tests = std::stoi(chunks[1]);
        int err_type = -1;

        chunks[2] = to_upper(chunks[2]);

        if(chunks[2] == "ERRNONE")        { err_type = 0; }
        else if(chunks[2] == "ERREXCEPT") { err_type = 1; }
        else if(chunks[2] == "ERRPARSE")  { err_type = 2; }
        else if(chunks[2] == "ERRTOKEN")  { err_type = 3; }

        assert(err_type > -1);

        std::cout << MAG <<  "Performing " << YEL << n_tests << MAG << " tests\n" << RST << std::flush;

        for(int i = 0; i < n_tests; i++) {
            std::getline(input_stream, line_contents);
            auto test_string = strip_line(line_contents);

            if(print_test_instruction)
                std::cout << test_string << std::endl << std::flush;

            std::ofstream ofile("test_script.txt", std::ios_base::out);
            ofile << test_string << std::endl << std::flush;
            ofile.close();

            std::string cmd = "./test_child test_script.txt " + std::to_string(print_exception_data);
            int _ = system(cmd.c_str());
            int returned_value = read_error_code_file();

            if(returned_value == err_type) {
                std::cout << GRN << "PASS" << RST << " : expected " << YEL << error_type_lut.at(err_type) 
                << RST << " got " << YEL << error_type_lut.at(returned_value) << RST << std::endl;

                total_passed_tests++;
                total_tests++;
            }
            else {
                std::cout << RED << "FAIL" << RST << " : expected " << YEL << error_type_lut.at(err_type) 
                << RST << " got " << YEL << error_type_lut.at(returned_value) << RST << std::endl;

                total_failed_tests++;
                total_tests++;

                if(fail_on_exception) {
                    print_testing_results(total_passed_tests, total_failed_tests, total_tests);
                    return 1;
                }
            }
        }
    }

    print_testing_results(total_passed_tests, total_failed_tests, total_tests);
    return 0;
}

void print_testing_results(int passed, int failed, int total) {
    std::cout << CYN << "\nTotal tests performed: " << RST << total << std::endl << std::flush;
    std::cout << GRN << "    Passed: " << RST << passed << std::endl << std::flush;
    std::cout << RED << "    Failed: " << RST << failed << std::endl << std::flush;
}

int read_error_code_file(void) {
    std::ifstream ifile("assembly_error_output.txt", std::ios_base::in);
    int err;
    ifile >> err;
    ifile.close();
    return err;
}

std::string to_upper(std::string s) {
    for(int i = 0; i < s.size(); i++) {
        if(s[i] >= 'a' && s[i] <= 'z') {
            s[i] -= 32;
        }
    }

    return s;
}

std::string strip_line(std::string s) {
    while(s.back() == '\n' || s.back() == '\r')
        s.pop_back();

    return s;
}

std::vector<std::string> split_line(std::string s) {
    std::vector<std::string> chks;
    std::string tmp;

    for(char c : s) {
        if(c == ' ') {
            chks.push_back(tmp);
            tmp.clear();
        }
        else {
            tmp.push_back(c);
        }
    }

    chks.push_back(tmp);
    return chks;
}
