#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "errcodes.h"


void throwErr(int errorCode, char* Filename, int SWFVersion) {

    switch( errorCode ) {

        case 11:
            std::cerr << "File: " << Filename << " is not in the SWF(Shock Wave Flash) format! " << "\nPlease use an swf file!\n";
            exit(11);
        break;

        case 21:
            std::cerr << "File: " << Filename << " Has an invalid version! File may be corrupted, or improperly created! Please use another file!\n";
            exit(21);
        break;

        case 31:
            std::cerr << "File: " << Filename << " Was compressed with LZMA, but current SWF version: " << SWFVersion << ". Does not support LZMA compression!(Requires At Least SWF 13+)\n";
            exit(31);
        break;
        
        case 32:
            std::cerr << "File: " << Filename << " Was compressed with ZLIB, but current SWF version: " << SWFVersion << ". Does not support ZLIB compression!(Requires At Least SWF 6+)\n";
            exit(32);
        break;

        case 41:
            std::cerr << "ZLIB Decompression Failed With Error: ERRNO!\n";
            exit(41);
        break;

        case 42:
            std::cerr << "ZLIB Decompression Failed With Error: STREAM_ERROR!\n";
            exit(42);
        break;

        case 43:
            std::cerr << "ZLIB Decompression Failed With Error: DATA_ERROR!\n";
            exit(43);
        break;

        case 44:
            std::cerr << "ZLIB Decompression Failed With Error: MEM_ERROR!\n";
            exit(44);
        break;

        case 45:
            std::cerr << "ZLIB Decompression Failed With Error: BUF_ERROR!\n";
            exit(45);
        break;

        case 46:
            std::cerr << "ZLIB Decompression Failed With Error: VERSION_ERROR!\n";
            exit(46);
        break;

        case 51:
            std::cerr << "LZMA Initialization failed with Error: MEM_ERROR!\n";
        break;

        case 52:
            std::cerr << "LZMA Initialization failed with Error: OPTIONS_ERROR!\n";
        break;

        case 53:
            std::cerr << "LZMA Decompression failed with Error: DATA_ERROR!\n";
        break;

        case 54:
            std::cerr << "LZMA Decompression failed with Error: BUF_ERROR!\n";
        break;

        case 55:
            std::cerr << "LZMA Decompression failed with Error: MEM_ERROR (during decode)!\n";
        break;


    }

}