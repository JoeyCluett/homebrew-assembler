
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include <inc/tokenize.h>
#include <inc/assemblererror.h>
#include <inc/parser.h>
#include <inc/mnemonics.h>
#include <inc/register_defs.h>

static long int convert_bin_string(const std::string& s) {
    std::string t = s.substr(2);
    std::reverse(t.begin(), t.end());

    long int res = 0L;
    int idx = 0;

    for(char c : t) {
        if(c == '1') res |= (1 << idx);
        idx++;
    }

    return res;
}

static long int convert_hex_string(const std::string& s) {
    std::string t = s.substr(2);
    std::reverse(t.begin(), t.end());

    long int res = 0L;
    int idx = 0;

    for(char c : t) {

        long int dig = -1L;

        if(c >= '0' && c <= '9') { dig = c - '0'; }
        else if(c >= 'a' && c <= 'f') { dig = c - 'a' + 10; }
        else if(c >= 'A' && c <= 'F') { dig = c - 'A' + 10; }

        if(dig < 0) {
            throw std::runtime_error("convert_hex_string : unknown internal error");
        }

        res |= (dig << idx);

        // 4 bits / digit
        idx += 4;
    }

    return res;
}

static long int convert_dec_string(const std::string& s) {
    return std::stol(s);
}

static long int convert_string_to_long(std::vector<char>& src, const token_t& tok) {
    std::string s = tok.str(src);
    long int val = -1;

    switch(tok.type) {
        case token_number_ord:
            val = convert_dec_string(s);
            break;
        case token_number_hex:
            val = convert_hex_string(s);
            break;
        case token_number_bin:
            val = convert_bin_string(s);
            break;
    }

    if(val > 255) {
        throw ParseException({
            "parse_add : token '" + tok.str(src) + "' of type '" + tok.typestr() + 
            "' is out of range (0 - 255). Numeric value is " + std::to_string(val) + ".",
            tok.idxstart
        });
    }
    else {
        return val;
    }
}

static bool token_is_number_type(const token_t& tok) {
    return
        tok.type == token_number_ord || 
        tok.type == token_number_hex || 
        tok.type == token_number_bin;
}

// functions return true when full instruction is parsed out
static bool parse_add(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_adc(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_sub(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_sbb(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_or(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_and(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_xor(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_not(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_inc(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_dec(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_rsh(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_rsh(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_lsh(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_sx(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_zx(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_cmp(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_mov(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);
static bool parse_jmp(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir);

void consume_token(GeneratedIR& gir, std::vector<char>& src, token_t tok) {

    static ParsedIR pir;

    // lottsa states

    const int state_default = 0;
    const int state_add     = 1;
    const int state_adc     = 2;
    const int state_sub     = 3;
    const int state_sbb     = 4;
    const int state_or      = 5;
    const int state_and     = 6;
    const int state_xor     = 7;
    const int state_not     = 8;
    const int state_inc     = 9;
    const int state_dec     = 10;
    const int state_rsh     = 11;
    const int state_lsh     = 12;
    const int state_sx      = 13;
    const int state_zx      = 14;
    const int state_cmp     = 15;
    const int state_mov     = 16;
    const int state_jmp     = 17;

    static int state_current = state_default;

    static const std::map<std::string, const int> keyword_state_xlat = {
        { "add", state_add }, // DONE
        { "adc", state_adc },
        { "sub", state_sub },
        { "sbb", state_sbb },
        { "or",  state_or },
        { "and", state_and },
        { "xor", state_xor },
        { "not", state_not },
        { "inc", state_inc },
        { "dec", state_dec },
        { "rsh", state_rsh },
        { "lsh", state_lsh },
        { "sx",          state_sx },
        { "sign_ext",    state_sx },
        { "zx",          state_zx },
        { "zero_extend", state_zx },
        { "cmp",         state_cmp },
        { "mov",  state_mov },
        { "move", state_mov },
        { "jmp",  state_jmp },
        { "jump", state_jmp },
    };

    switch(state_current) {
        case state_default:
            pir.type = PARSE_TYPE_INVALID;
            {
                auto str = tok.str(src);
                if(tok.type == token_instruction) {
                    auto keyword_iter = keyword_state_xlat.find(str);
                    if(keyword_iter != keyword_state_xlat.end()) {
                        state_current = keyword_iter->second;
                    }
                }
            }
            break;

        case state_add: if(parse_add(gir, src, tok, pir)) { state_current = state_default; } break;
        case state_mov: if(parse_mov(gir, src, tok, pir)) { state_current = state_default; } break;

        //case state_or:
        //    if(parse_or(gir, src, tok, pir)) { state_current = state_default; }

        default:
            PARSE_EXCEPTION_UNKNOWN_ERROR(consume_token, tok);
    }

}

static bool parse_mov(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir) {
    //
    // mov rd, rs        PARSE_INTERNAL_MOV_RR
    // mov rd, [rs]      PARSE_INTERNAL_MOV_LOAD
    // mov [rd], rs      PARSE_INTERNAL_MOV_STORE
    // mov rd, 0xFFFF    PARSE_INTERNAL_MOV_LOAD_IMM
    //
    //                     --> reg
    //                     |
    // mov ---> reg -----> , ---> [ --> reg --> ]
    //  |                  |
    //  |                  --> num
    //  |
    //  --> [ ---> reg --> ] --> , --> reg
    //

    const int state_rd_expect_reg_or_openbr            = 0; // dest expect either opening bracket or register reference

    const int state_rd_reg_expect_comma                = 1; // dest is register, expect comma
    const int state_rd_reg_expect_reg_or_openbr_or_num = 2;
    const int state_rd_reg_rs_expect_reg               = 3;
    const int state_rd_reg_rs_expect_closebr           = 4;

    const int state_rd_mem_expect_reg                  = 5;
    const int state_rd_mem_expect_closebr              = 6;
    const int state_rd_mem_expect_comma                = 7;
    const int state_rd_mem_rs_expect_reg               = 8;

    static int state_current = state_rd_expect_reg_or_openbr;

    switch(state_current) {
        case state_rd_expect_reg_or_openbr: // mov *reg|[*
            if(tok.type == token_register) {
                int rd = register_refs.at(tok.str(src));
                state_current = state_rd_reg_expect_comma;
                pir.type = PARSE_TYPE_MOV; // all move instructions are of type 'mov'
                pir.mov.rd = rd;
            }
            else if(tok.type == token_openbr) {
                state_current = state_rd_mem_expect_reg;
                pir.type = PARSE_TYPE_MOV;
                pir.mov.type = PARSE_INTERNAL_MOV_STORE;
            }
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Register' or 'OpenBracket');
            break;
        case state_rd_reg_expect_comma: // mov reg *,*
            if(tok.type == token_comma) {
                state_current = state_rd_reg_expect_reg_or_openbr_or_num;
            }
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Comma' or 'Register');
            break;
        case state_rd_reg_expect_reg_or_openbr_or_num: // mov reg, *reg|[|num*
            if(tok.type == token_register) {
                // DONE (*)
                state_current = state_rd_expect_reg_or_openbr;
                int rs = register_refs.at(tok.str(src));
                pir.mov.type = PARSE_INTERNAL_MOV_RR;
                pir.mov.rs = rs;
                gir.ir.push_back(pir);
                return true;
            }
            else if(tok.type == token_openbr) {
                state_current = state_rd_reg_rs_expect_reg;
                pir.mov.type = PARSE_INTERNAL_MOV_LOAD;
            }
            else if(token_is_number_type(tok)) {
                // DONE (*)
                state_current = state_rd_expect_reg_or_openbr;
                long int imm = convert_string_to_long(src, tok);
                pir.mov.type = PARSE_INTERNAL_MOV_LOAD_IMM;
                pir.mov.imm = imm & 0xFFFF;
                gir.ir.push_back(pir);
                return true;
            }
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Register' or 'OpenBracket' or 'Number*');
            break;
        case state_rd_reg_rs_expect_reg: // mov reg, [ *reg*
            if(tok.type == token_register) {
                state_current = state_rd_reg_rs_expect_closebr;
                int rs = register_refs.at(tok.str(src));
                pir.mov.rs = rs;
            }
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Register');
            break;
        case state_rd_reg_rs_expect_closebr: // mov reg, [reg *]*
            if(tok.type == token_closebr) {
                // DONE
                state_current = state_rd_expect_reg_or_openbr;
                gir.ir.push_back(pir);
                return true;
            }
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'CloseBracket');
            break;
        case state_rd_mem_expect_reg: // mov [ *reg*
            if(tok.type == token_register)
                state_current = state_rd_mem_expect_closebr;
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Register');
            break;
        case state_rd_mem_expect_closebr: // mov [reg *]*
            if(tok.type == token_closebr)
                state_current = state_rd_mem_expect_comma;
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'CloseBracket');
            break;
        case state_rd_mem_expect_comma: // mov [reg] *,*
            if(tok.type == token_comma)
                state_current = state_rd_mem_rs_expect_reg;
            else
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Comma');
            break;
        case state_rd_mem_rs_expect_reg: // mov [reg], *reg*
            if(tok.type == token_register) {
                // DONE
                state_current = state_rd_expect_reg_or_openbr;
                int rs = register_refs.at(tok.str(src));
                pir.mov.rs = rs;
                gir.ir.push_back(pir);
                return true;
            }
            else {
                // ERROR
                PARSE_EXCEPTION_WRONG_TOKEN_TYPE(parse_mov, tok, 'Register');
            }
            break;
        default:
            // ERROR
            PARSE_EXCEPTION_UNKNOWN_ERROR(parse_mov, tok);
            break;        
    }

    return false;
}

static bool parse_add(GeneratedIR& gir, std::vector<char>& src, token_t tok, ParsedIR& pir) {
    //                     rd               rs 
    // add --------------> REG --> , -----> REG
    //       |              ^      |
    //       ---> .byte ----|      ---> NUMBER


    const int state_reg_or_bytespec = 0;
    const int state_reg_folow       = 1;
    const int state_comma           = 2;
    const int state_reg_or_number   = 3;

    static int state_current = state_reg_or_bytespec;

    switch(state_current) {
        case state_reg_or_bytespec:
            if(tok.type == token_register) {
                int rd = register_refs.at(tok.str(src)); // assume the register reference is valid
                pir.type = PARSE_TYPE_ALU_STD;
                pir.alu_std.rd = rd;
                state_current = state_comma;
                return false;
            }
            else if(tok.type == token_bytespec) {
                pir.type = PARSE_TYPE_ALU_TR;
                state_current = state_reg_folow;
                return false;
            }
            else {
                throw ParseException({
                    "parse_add : unexpected token '" + tok.str(src) + "' of type '" + tok.typestr() + 
                    "'. Expecting token of type 'ByteSpec' or 'Register'.",
                    tok.idxstart
                });
            }
            break;
        case state_reg_folow:
            if(tok.type == token_register) {
                int rd = register_refs.at(tok.str(src));
                pir.alu_tr.rd = rd;
                state_current = state_comma;
                return false;
            }
            else {
                throw ParseException({
                    "parse_add : unexpected token '" + tok.str(src) + "' of type '" + tok.typestr() + 
                    "'. Expecting token of type 'Register'.",
                    tok.idxstart
                });
            }
            break;
        case state_comma:
            if(tok.type == token_comma) {
                state_current = state_reg_or_number;
                return false;
            }
            else {
                throw ParseException({
                    "parse_add : unexpected token '" + tok.str(src) + "' of type '" + tok.typestr() + 
                    "'. Expecting token of type 'Comma'.",
                    tok.idxstart
                });
            }
            break;
        case state_reg_or_number:
            if(token_is_number_type(tok)) {
                if(pir.type == PARSE_TYPE_ALU_TR) {
                    throw ParseException({
                        "parse_add : token '" + tok.str(src) + "' is invalid for add instruction with '.byte' "
                        "modifier. Immediates are only valid for 16-bit operands.",
                        tok.idxstart
                    });
                }
            }

            if(tok.type == token_register) {
                int rs = register_refs.at(tok.str(src));
                if(pir.type == PARSE_TYPE_ALU_TR) {
                    pir.alu_tr.rs = rs; 
                    pir.alu_tr.op = PARSE_INTERNAL_ALU_OP_ADD_RR; // must be true for truncated operations
                }
                else { 
                    pir.alu_std.rs = rs; 
                    pir.alu_std.op = PARSE_INTERNAL_ALU_OP_ADD_RR;
                }

                state_current = state_reg_or_bytespec;
                gir.ir.push_back(pir);
                return true;
            }
            else if(token_is_number_type(tok)) {
                long int val = convert_string_to_long(src, tok);
                pir.alu_std.imm = (int)(val & 0xFF);
                pir.alu_std.op  = PARSE_INTERNAL_ALU_OP_ADD_IMM8;

                state_current = state_reg_or_bytespec;
                gir.ir.push_back(pir);
                return true;
            }
            else {
                throw ParseException({
                    "parse_add : unexpected token '" + tok.str(src) + "' of type '" + tok.typestr() + 
                    "'. Expecting token of type 'Number*' or 'Register'.",
                    tok.idxstart
                });
            }
            break;
        default:
            throw std::runtime_error("parse_add : UNKNOWN INTERNAL ERROR");
    }

}
