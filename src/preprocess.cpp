#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_set>
#include <stdio.h>
#include <unistd.h>

#include <inc/preprocess.h>

static std::vector<char> strip_comments(std::vector<char>& v) {
    std::vector<char> r;

    const int state_default = 0;
    const int state_comment = 1;

    int state_current = state_default;

    for(char c : v) {
        switch(state_current) {
        case state_default:
            if(c == '#') {
                r.push_back(' ');
                state_current = state_comment;
            }
            else {
                r.push_back(c);
            }
            break;
        case state_comment:
            if(c == '\n') {
                r.push_back(c);
                state_current = state_default;
            }
            else {
                // replace comments with spaces
                r.push_back(' ');
            }
            break;
        default:
            throw std::runtime_error( "strip_comments : unknown internal error" );
        }
    }

    return r;
}

std::vector<char> read_file(const std::string& fname) {
    std::vector<char> v;

    FILE* fptr = fopen( fname.c_str(), "r" );

    char buf[1024];

    int nread = fread( buf, 1, 1024, fptr );

    while(nread > 0) {

        for(int i = 0; i < nread; i++)
            v.push_back( buf[i] );

        nread = fread( buf, 1, 1024, fptr );
    }

    fclose(fptr);
    return strip_comments(v);
}

