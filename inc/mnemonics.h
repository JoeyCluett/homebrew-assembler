#pragma once

#include <unordered_set>
#include <string>

const std::unordered_set<std::string> instruction_mnemonics = {
    "add", "adc", "sub", "sbb", 
    "or", "and", "xor", "not", 
    "inc", "dec", "lsh", "rsh", 
    "sign_ext", "sx", "se",
    "zero_ext", "zx", "ze",
    "nop", 
    "zero",
    "comp", "cmp",
    "jump", "jmp",
    "move", "mov",
};



