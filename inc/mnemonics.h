#pragma once

#include <unordered_set>
#include <string>

std::unordered_set<std::string> instruction_mnemonics = {
    "add", "adc", "sub", "sbb", 
    "or", "and", "xor", "not", 
    "inc", "dec", "lsh", "rsh", 
    "sign_ext", "sx", 
    "zero_ext", "zx",
    "nop", "zero",
    "comp", "cmp",
    "jump", "jmp",
};



