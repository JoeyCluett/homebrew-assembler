#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <inc/tokenize.h>
#include <inc/mnemonics.h>
#include <inc/register_defs.h>
#include <inc/tokenerror.h>

std::string token_t::str(std::vector<char>& src) {
    char* srcptr = src.data();

    return std::string( 
            srcptr + this->idxstart, 
            srcptr + this->idxend);
}

std::string token_t::typestr(void) {
    switch(this->type) {
        case token_register:    return "Register     ";
        case token_instruction: return "Instruction  ";
        case token_comma:       return "Comma        ";
        case token_bytespec:    return "ByteSpec     ";
        case token_flagspec:    return "FlagSpec     ";
        case token_labelspec:   return "LabelSpec    ";

        default:                return "UNKNOWN_TYPE ";

    }
}

static bool is_alpha(char c) { return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')); }
static bool is_number(char c) { return ((c >= '0') && (c <= '9')); }
static bool is_alphanum(char c) { return is_alpha(c) || is_number(c); }

static bool is_valid_flags_specifier(const std::string& flag_spec) {

    if(flag_spec[0] != '.')
        return false;

    for(int i = 1; i < flag_spec.size(); i++) {
        char c = flag_spec[i];

        // ZCEV
        if(c != 'Z' && c != 'C' && c != 'E' && c != 'V')
            return false;
    }

    return true;
}

token_t get_token(std::vector<char>& src, std::vector<char>::iterator& iter) {

    token_t tok;
    tok.valid_token = false;

    static std::vector<char>::iterator token_start_loc = src.begin();
    static std::string char_buffer;

    const int state_default = 0;
    const int state_word    = 1; // instruction mnemonics, register refs, labels, other keywords
    const int state_dotword = 2;
    const int state_label   = 3;
    const int state_comma   = 4;
    static int state_current = state_default;

    while(iter != src.end()) {
        char c = *iter;

        switch(state_current) {
            case state_default:
                if(c == '\n') {
                    iter++;
                }
                else if(is_alpha(c) || c == '_') {
                    state_current = state_word;
                    token_start_loc = iter;
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(c == '.') {
                    state_current = state_dotword;
                    token_start_loc = iter;
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(c == '@') {
                    state_current = state_label;
                    token_start_loc = iter;
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(c == ',') {
                    state_current = state_comma;
                    token_start_loc = iter;
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(c == ' ') {
                    iter++;
                }
                else {
                    char_buffer = c;
                    token_start_loc = iter;
                    throw TokenizeException({
                            "state_word : invalid char value '" + char_buffer + "'", 
                            token_start_loc - src.begin()});
                }
                break;

            case state_word:
                // may not need to test for '_'
                if(is_alphanum(c) || c == '_') {
                    char_buffer.push_back(c);
                    iter++;
                }
                else {

                    //std::cout << char_buffer << std::endl;

                    state_current = state_default;

                    auto word_iter = register_refs.find(char_buffer);
                    if(word_iter != register_refs.end()) {
                        // word is a register reference
                        tok.idxstart = (int)(token_start_loc - src.begin());
                        tok.idxend = tok.idxstart + char_buffer.size();
                        tok.valid_token = true;
                        tok.type = token_register;
                        char_buffer.clear();
                        return tok;
                    }

                    auto inst_iter = instruction_mnemonics.find(char_buffer);
                    if(inst_iter != instruction_mnemonics.end()) {  
                        // word is an instruction mnemonic
                        tok.idxstart = (int)(token_start_loc - src.begin());
                        tok.idxend = tok.idxstart + char_buffer.size();
                        tok.valid_token = true;
                        tok.type = token_instruction;
                        char_buffer.clear();
                        return tok;
                    }

                    throw TokenizeException({
                            "state_word : invalid token value '" + char_buffer + "'",
                            token_start_loc - src.begin()});
                }
                break;

            case state_dotword:
                if(is_alphanum(c) || c == '_') {
                    char_buffer.push_back(c);
                    iter++;
                }
                else {

                    state_current = state_default;

                    if(char_buffer == ".byte") {
                        tok.idxstart = (int)(token_start_loc - src.begin());
                        tok.idxend = tok.idxstart + char_buffer.size();
                        tok.valid_token = true;
                        tok.type = token_bytespec;
                        char_buffer.clear();
                        return tok;
                    }
                    else if(is_valid_flags_specifier(char_buffer)) {
                        tok.idxstart = (int)(token_start_loc - src.begin());
                        tok.idxend = tok.idxstart + char_buffer.size();
                        tok.valid_token = true;
                        tok.type = token_bytespec;
                        char_buffer.clear();
                        return tok;
                    }
                    else {
                        throw TokenizeException({
                                "state_dotword : invalid token value '" + char_buffer + "'",
                                token_start_loc - src.begin()});
                    }

                }
                break;

            case state_label:
                if(is_alphanum(c) || c == '_') {
                    char_buffer.push_back(c);
                    iter++;
                }
                else {
                    state_current = state_default;

                    tok.idxstart = (int)(token_start_loc - src.begin());
                    tok.idxend = tok.idxstart + char_buffer.size();
                    tok.valid_token = true;
                    tok.type = token_labelspec;
                    char_buffer.clear();
                    return tok;
                }
                break;

            case state_comma:
                state_current = state_default;

                tok.idxstart = (int)(token_start_loc - src.begin());
                tok.idxend = tok.idxstart + char_buffer.size();
                tok.valid_token = true;
                tok.type = token_comma;
                char_buffer.clear();
                return tok;

            default:
                std::cerr << "UNKNOWN INTERNAL ERROR" << std::endl;
                exit(1);
                break;
        }
    }
}

