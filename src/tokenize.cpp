#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <inc/tokenize.h>
#include <inc/mnemonics.h>
#include <inc/register_defs.h>
#include <inc/assemblererror.h>

std::string token_t::str(std::vector<char>& src) const {
    char* srcptr = src.data();

    return std::string( 
            srcptr + this->idxstart, 
            srcptr + this->idxend);
}

std::string token_t::typestr(void) const {
    switch(this->type) {
        case token_register:    return "Register";
        case token_instruction: return "Instruction";
        case token_comma:       return "Comma";
        case token_bytespec:    return "ByteSpec";
        case token_flagspec:    return "FlagSpec";
        case token_labelspec:   return "LabelSpec";
        case token_openbr:      return "OpenBracket";
        case token_closebr:     return "CloseBracket";
        case token_number_ord:  return "NumberDecimal";
        case token_number_bin:  return "NumberBinary";
        case token_number_hex:  return "NumberHexadecimal";

        default:                return "UNKNOWN_TYPE  ";

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
    const int state_bracket_open  = 5;
    const int state_bracket_close = 6;
    const int state_number_ord    = 7; // number doesnt start with 0 (ordinary)
    const int state_number_spec   = 8; // number starts with 0 (special)
    const int state_num_hex = 9;
    const int state_num_bin = 10;
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
                else if(is_number(c)) {
                    if(c == '0')
                        state_current = state_number_spec;
                    else
                        state_current = state_number_ord;
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
                else if(c == '[') {
                    state_current = state_bracket_open;
                    token_start_loc = iter;
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(c == ']') {
                    state_current = state_bracket_close;
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
                        tok.type = token_flagspec;
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

            case state_bracket_open:
                state_current = state_default;

                tok.idxstart = (int)(token_start_loc - src.begin());
                tok.idxend = tok.idxstart + char_buffer.size();
                tok.valid_token = true;
                tok.type = token_openbr;
                char_buffer.clear();
                return tok;

            case state_bracket_close:
                state_current = state_default;

                tok.idxstart = (int)(token_start_loc - src.begin());
                tok.idxend = tok.idxstart + char_buffer.size();
                tok.valid_token = true;
                tok.type = token_closebr;
                char_buffer.clear();
                return tok;

            case state_number_ord:
                if(is_number(c)) {
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(is_alpha(c) || c == '_') {
                    char_buffer = c;
                    throw TokenizeException({
                        "state_number_ord : invalid char '" + char_buffer + "' in decimal number",
                        iter - src.begin() // one char past the start of the number
                    });
                }
                else {
                    state_current = state_default;

                    tok.idxstart = (int)(token_start_loc - src.begin());
                    tok.idxend = tok.idxstart + char_buffer.size();
                    tok.valid_token = true;
                    tok.type = token_number_ord;
                    char_buffer.clear();
                    return tok;
                }
                break;

            case state_number_spec:
                if(c == 'x') {
                    state_current = state_num_hex;
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(c == 'b') {
                    state_current = state_num_bin;
                    char_buffer.push_back(c);
                    iter++;
                }
                else {
                    char_buffer = c;
                    throw TokenizeException({
                        "state_number_spec : invalid special char '" + char_buffer + "' in number, expecting either 'b' or 'x'",
                        token_start_loc - src.begin() + 1 // one char past the start of the number
                    });
                }
                break;

            case state_num_hex:
                if(is_number(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')) ) {
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(is_alpha(c) || c == '_') {
                    char_buffer = c;
                    throw TokenizeException({
                        "state_num_hex : invalid char '" + char_buffer + "' in hexadecimal number",
                        iter - src.begin() // one char past the start of the number
                    });
                }
                else {
                    state_current = state_default;

                    tok.idxstart = (int)(token_start_loc - src.begin());
                    tok.idxend = tok.idxstart + char_buffer.size();
                    tok.valid_token = true;
                    tok.type = token_number_hex;
                    char_buffer.clear();
                    return tok;
                }
                break;

            case state_num_bin:
                if(c == '0' || c == '1') {
                    char_buffer.push_back(c);
                    iter++;
                }
                else if(is_alphanum(c) || c == '_') {
                    char_buffer = c;
                    throw TokenizeException({
                        "state_num_bin : invalid char '" + char_buffer + "' in binary number",
                        iter - src.begin() // one char past the start of the number
                    });
                }
                else {
                    state_current = state_default;

                    tok.idxstart = (int)(token_start_loc - src.begin());
                    tok.idxend = tok.idxstart + char_buffer.size();
                    tok.valid_token = true;
                    tok.type = token_number_bin;
                    char_buffer.clear();
                    return tok;
                }
                break;

            default:
                std::cerr << "UNKNOWN INTERNAL ERROR" << std::endl;
                exit(1);
                break;
        }
    }
}

