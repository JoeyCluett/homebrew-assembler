#pragma once

#include <map>
#include <string>
#include <vector>
#include <unordered_set>

#include "tokenize.h"

const int PARSE_TYPE_INVALID = -1; // used during the parse/ir-gen phase. indicates incomplete token-sequence
const int PARSE_TYPE_ALU_STD = 0; // 16-bit operations
const int PARSE_TYPE_ALU_TR  = 1; // 8-bit operations (truncated) 
const int PARSE_TYPE_MOV     = 3; // all move operations
const int PARSE_TYPE_JMP     = 4; // branches

// internal mov type flags
const int PARSE_INTERNAL_MOV_RR       = 0; // mov register to register
const int PARSE_INTERNAL_MOV_LOAD     = 1; // mov mem to register
const int PARSE_INTERNAL_MOV_STORE    = 2; // mov register to mem
const int PARSE_INTERNAL_MOV_LOAD_IMM = 3; // mov immediate to register

// internal jmp type flags
const int PARSE_INTERNAL_JMP_REL     = 0; // conditional short branch
const int PARSE_INTERNAL_JMP_ABS     = 1; // conditional long branch
const int PARSE_INTERNAL_JMP_UNC_REL = 2; // unconditional short branch
const int PARSE_INTERNAL_JMP_UNC_ABS = 3; // unconditional long branch

// internal alu operations flags
const int PARSE_INTERNAL_ALU_OP_ADD_RR   = 0;
const int PARSE_INTERNAL_ALU_OP_ADD_IMM8 = 1;
const int PARSE_INTERNAL_ALU_OP_ADC      = 2;
const int PARSE_INTERNAL_ALU_OP_SUB      = 3;
const int PARSE_INTERNAL_ALU_OP_SBB      = 4;
const int PARSE_INTERNAL_ALU_OP_OR       = 5;
const int PARSE_INTERNAL_ALU_OP_AND      = 6;
const int PARSE_INTERNAL_ALU_OP_XOR      = 7;
const int PARSE_INTERNAL_ALU_OP_NOT      = 8;
const int PARSE_INTERNAL_ALU_OP_INC      = 9;
const int PARSE_INTERNAL_ALU_OP_DEC      = 10;
const int PARSE_INTERNAL_ALU_OP_LSH      = 11;
const int PARSE_INTERNAL_ALU_OP_RSH      = 12;
const int PARSE_INTERNAL_ALU_OP_SX       = 13;
const int PARSE_INTERNAL_ALU_OP_ZX       = 14;
const int PARSE_INTERNAL_ALU_OP_CMP      = 15;
const int PARSE_INTERNAL_ALU_OP_MOV      = 16;

struct ParsedIR {

    int type;

    union {

        struct {
            int op; // one of Parse_type_alu_op_*
            int rd;
            union {
                int rs;
                int imm;
            };
        } alu_std;

        struct {
            int op; // one of Parse_type_alu_op_*
            int rd;
            union {
                int rs;
                int imm;
            };
        } alu_tr;

        struct {
            int type; // one of Parse_type_mov_*
            int rd;
            int rs;
        } mov;

        struct {
            int type; // one of Parse_type_jmp_*
            int flags;
            int target;
        } jmp;

    };
};

struct GeneratedIR {

    // when the parse run is over, both of these should be filled out
    std::map<std::string, int> label_to_target;
    std::map<int, std::string> target_to_label;

    // not used yet, but will be
    std::unordered_set<std::string> exported_targets;

    // list of instruction representations
    // needed to optimize jump target types in the code gen stage
    std::vector<ParsedIR> ir;

};

void consume_token(GeneratedIR& gir, std::vector<char>& src, token_t tok);
