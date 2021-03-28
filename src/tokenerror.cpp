#include <inc/tokenerror.h>

#include <iostream>
#include <vector>

void generate_tokenizer_error(
        std::vector<char>& src, 
        long int offset, 
        std::string message, 
        std::string filename) {

    auto offset_iter = src.begin() + offset;

    std::vector<long int> row_offsets = { 0 }; // row_offets[r] = row start location

    int row = 0;
    int col = 0;
    auto iter = src.begin();

    while(iter != src.end()) {
        char c = *iter;

        if(iter == offset_iter)
            break;

        switch(c) {
            case '\r':
                break;
            case '\n':
                row++;
                col = 0;
                row_offsets.push_back( (iter + 1) - src.begin() );
                break;
            default:
                col++;
                break;
        }

        iter++;
    }

    if(filename != "") {
        std::cout << "\n In file '" << filename << "'\n";
    }

    std::cout << std::endl << "  " << message << std::endl;
    std::cout << "  Ln " << (row + 1) << ", Col " << (col + 1) << "\n  code:\n\n"; 

    iter = src.begin() + row_offsets[row];
    while(*iter != '\n' && iter != src.end()) {
        std::cout << *iter;
        iter++;
    }
    std::cout << std::endl;

    long int empty_spaces = offset - row_offsets[row];
    for(long int i = 0; i < empty_spaces; i++)
        std::cout << ' ';
    std::cout << "^\n\n";

}
