#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "header/header.h"
#include "main.h"

int main(int argc, char* argv[]) {

    std::cout << argv[1] << "\n";
    std::vector<uint8_t> inputFile = readFile(argv[1]);
    
    getSWFHeader(inputFile, argv[1]);

}