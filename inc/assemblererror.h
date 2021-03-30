#pragma once

#include <string>
#include <vector>

struct TokenizeException {
    std::string message;
    long int offset;
};

struct ParseException {
    std::string message;
    long int offset;
};

void generate_assembler_error(
        const std::string& error_type, 
        std::vector<char>& src, 
        long int offset, 
        std::string message, 
        std::string filename);
