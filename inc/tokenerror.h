#pragma once

#include <vector>
#include <string>

struct TokenizeException {
    std::string message;
    long int offset;
};

void generate_tokenizer_error(std::vector<char>& src, long int offset, std::string message, std::string filename="");


