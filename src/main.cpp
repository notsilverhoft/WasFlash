#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "header/header.h"
#include "tags/tags.h"
#include "main.h"

int main(int argc, char* argv[]) {

    std::cout << argv[1] << "\n";
    std::vector<uint8_t> inputFile = readFile(argv[1]);
    
    SWFHeader header = getSWFHeader(inputFile, argv[1]);

    std::vector<uint8_t> SWFFile = header.SWFFile; 

    getSWFTag(SWFFile);

}