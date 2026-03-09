#include <iostream>
#include <cstdint>
#include <fstream> 
#include <string>
#include <vector>
#include <iterator>


    std::vector<uint8_t> readFile(const char* filePath) {

        std::ifstream rawFile(filePath);
        rawFile.unsetf(std::ios::skipws);
        std::streampos fileSize;
        rawFile.seekg(0, std::ios::end);
        fileSize = rawFile.tellg();
        rawFile.seekg(0, std::ios::beg);

        std::vector<uint8_t> fileStreamOut;
        fileStreamOut.reserve(fileSize);
        fileStreamOut.insert(fileStreamOut.begin(),
            std::istream_iterator<uint8_t>(rawFile),
            std::istream_iterator<uint8_t>());

        return fileStreamOut;
        
}

