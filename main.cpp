#include <iostream>
#include <stdio.h>

#include "inc/preprocess.h"
#include "inc/tokenize.h"
#include "inc/tokenerror.h"

using namespace std;

int main(int argc, char* argv[]) {

    std::string filename = "examples/helloworld.txt";

    auto vchar  = read_file(filename);
    //cout.write(vchar.data(), vchar.size());

    auto iter = vchar.begin();

    try {

        auto token = get_token(vchar, iter);
        while(token.valid_token && iter != vchar.end()) {
            
            //cout << token.typestr() << token.str(vchar) << endl;
            //cout << token.idxstart << " - " << token.idxend << endl;

            // process token...

            // get next token
            token = get_token(vchar, iter);
        }

    }
    catch(TokenizeException& te) {
        generate_tokenizer_error(vchar, te.offset, te.message, filename);
    }

    return 0;
}
