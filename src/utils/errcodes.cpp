#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "errcodes.h"


void throwErr(int errorCode, const char* Filename, int SWFVersion) {

    switch( errorCode ) {

        case 11:
            std::cerr << "File: " << Filename << " is not in the SWF(Shock Wave Flash) format! " << "\nPlease use an swf file!\n";
            exit(11);
        break;

        case 21:
            std::cerr << "File: " << Filename << " Has an invalid version( " << SWFVersion << ")! File may be corrupted, or improperly created! Please use another file!\n";
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
            exit(51);
        break;

        case 52:
            std::cerr << "LZMA Initialization failed with Error: OPTIONS_ERROR!\n";
            exit(52);
        break;

        case 53:
            std::cerr << "LZMA Decompression failed with Error: DATA_ERROR!\n";
            exit(53);
        break;

        case 54:
            std::cerr << "LZMA Decompression failed with Error: BUF_ERROR!\n";
            exit(54);
        break;

        case 55:
            std::cerr << "LZMA Decompression failed with Error: MEM_ERROR (during decode)!\n";
            exit(55);
        break;

        case 181: // Tag #18 - Error #1
            std::cerr << "Error while processing SoundStreamHead tag: MP3 Audio Compression is only allowed with SWF Versions 4+(Current is " << SWFVersion << ")! CODE:181";
            exit(181);
        break;
       
        case 182: // Tag #18 - Error #2
            std::cerr << "Error while processing SoundStreamHead tag: Invalid Tag Params! -- StreamSoundSize is not 1 -- Your File is Likely Corrupted! CODE:182";
            exit(182);
        break;

        case 183: // Tag #18 - Error #3
            std::cerr << "Error while processing SoundStreamHead tag: Reserved Area is NOT Empty! Your File is Likely Corrupted! CODE:183\n";
            exit(183);
        break;

        case 451: // Tag #45 - Error #1
            std::cerr << "Error while processing SoundStreamHead2 tag: MP3 Audio Compression is only allowed with SWF Versions 4+(Current is " << SWFVersion << ")! CODE:451";
            exit(451);
        break;

        case 452: // Tag #45 - Error #2
            std::cerr << "Error while processing SoundStreamHead2 tag: Little Endian Uncompressed Audio is only allowed with SWF Versions 4+(Current is " << SWFVersion << ")! CODE:452";
            exit(452);
        break;
        
        case 453: // Tag #45 - Error #3
            std::cerr << "Error while processing SoundStreamHead2 tag: Nellymoser 16 kHz Audio Compression is only allowed with SWF Versions 10+(Current is " << SWFVersion << ")! CODE:453";
            exit(453);
        break;

        case 454: // Tag #45 - Error #4
            std::cerr << "Error while processing SoundStreamHead2 tag: Nellymoser 8 kHz Audio Compression is only allowed with SWF Versions 10+(Current is " << SWFVersion << ")! CODE:454";
            exit(454);
        break;

        case 455: // Tag #45 - Error #5
            std::cerr << "Error while processing SoundStreamHead2 tag: Nellymoser Audio Compression is only allowed with SWF Versions 6+(Current is " << SWFVersion << ")! CODE:455";
            exit(455);
        break;

        case 456: // Tag #45 - Error #6
            std::cerr << "Error while processing SoundStreamHead2 tag: Speex Audio Compression is only allowed with SWF Versions 10+(Current is " << SWFVersion << ")! CODE:456";
            exit(456);
        break;

        case 457: // Tag #18 - Error #7
            std::cerr << "Error while processing SoundStreamHead2 tag: Reserved Area is NOT Empty! Your File is Likely Corrupted! CODE:457\n";
            exit(457);
        break;

        case 601: // Tag #60 - Error #1
            std::cerr << "Error while processing DefineVideoStream tag: Reserved Area is NOT Empty! Your File is Likely Corrupted! CODE:601\n";
            exit(601);
        break;

        case 602: // Tag #60 - Error #2
            std::cerr << "Error while processing DefineVideoStream tag: One or more fields used require a newer SWF Version(Current is " << SWFVersion << ")! CODE:602\n";
            exit(602);
        break;
        
        case 603: // Tag #60 - Error #3
            std::cerr << "Error while processing DefineVideoStream tag: Invalid Codec ID! Your File is Likely Corrupted! CODE:603\n";
            exit(603);
        break;

        case 691: // Tag #69 - Error #1
            std::cerr << "Error while processing FileAttributes tag: Reserved Area is NOT Empty! Your File is Likely Corrupted! CODE:691\n";
            exit(691);
        break;

        case 692: // Tag #69 - Error #2
            std::cerr << "Error while processing FileAttributes tag: One or more fields used require a newer SWF Version(Current is " << SWFVersion << ")! CODE:692\n";
            exit(692);
        break;
    }

}