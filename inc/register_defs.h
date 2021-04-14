#pragma once

#include <unordered_set>
#include <map>
#include <string>

const std::map<std::string, int> register_refs = {
    { "r0", 0 },
    { "r1", 1 },
    { "r2", 2 },
    { "r3", 3 },
    { "r4", 4 },   { "a0", 4 },
    { "r5", 5 },   { "a1", 5 },
    { "r6", 6 },   { "a2", 6 },
    { "r7", 7 },   { "sp", 7 },
    { "r8", 8 },   { "t0", 8 },
    { "r9", 9 },   { "t1", 9 },
    { "r10", 10 }, { "t2", 10 },
    { "r11", 11 }, { "t3", 11 },
    { "r12", 12 }, { "t4", 12 },
    { "r13", 13 }, { "mul_lo", 13 }, { "mlo", 13 },
    { "r14", 14 }, { "mul_hi", 14 }, { "mhi", 14 },
    { "r15", 15 },
};

/*

    // registers are too sparse to justify a dedicated zero register
    // instruction pointer is a special hardware device and is not directly accessible from code

    a0
    a1
    a2

    t0
    t1
    t2
    t3
    t4

    mul_lo
    mul_hi

    sp // 16-bit stack pointer


*/

