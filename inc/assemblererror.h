#pragma once

#include <string>
#include <vector>

// yes, yes, abuse the preprocessor!
#define PARSE_EXCEPTION_WRONG_TOKEN_TYPE(function_name, token, expected_type) \
    throw ParseException({ \
        #function_name " : unexpected token '" + token.str(src) + "' of type '" + token.typestr() + \
        "'. Expecting token of type " #expected_type ".", \
        token.idxstart \
    })

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
