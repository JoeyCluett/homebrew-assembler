#include <iostream>
#include <string>
#include <vector>

#include <inc/assemblererror.h>
#include <iostream>

void generate_assembler_error(
        const std::string& error_type, 
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


    std::cout << "\n (" << error_type << ")\n In file '" << filename << "'\n";
    std::cout << " Ln " << (row + 1) << ", Col " << (col + 1) << "\n"; 

    std::cout << std::endl << "  " << message << std::endl;
    std::cout << "  code:\n\n"; 

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