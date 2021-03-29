#pragma once

#include <vector>
#include <string>

const int token_register    = 0;  //     register references (r0 - r15)
const int token_instruction = 1;  //     mnemonics
const int token_comma       = 2;  //     ,
const int token_bytespec    = 3;  //     .byte specifier
const int token_flagspec    = 4;  //     .CZEV flag specifier
const int token_labelspec   = 5;  //     @label specifier
const int token_openbr      = 6;  //     [
const int token_closebr     = 7;  //     ]
const int token_number_ord  = 8;  //     regular decimal number
const int token_number_bin  = 9;  //     binary number
const int token_number_hex  = 10; //     hexadecimal number

struct token_t {

    int valid_token;
    int type;
    int col;
    int row;
    int idxstart;
    int idxend;

    std::string str(std::vector<char>& src);
    std::string typestr(void);
};

token_t get_token(std::vector<char>& src, std::vector<char>::iterator& iter);


