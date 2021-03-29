#pragma once

#include <string>
#include <vector>
#include <iostream>

struct ParseException {
    std::string message;
    long int offset;
};

void generate_parser_error(std::vector<char>& src, long int offset, std::string message, std::string filename);
