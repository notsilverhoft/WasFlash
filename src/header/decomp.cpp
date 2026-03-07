#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "decomp.h"
#include "zlib.h"
#include "lzma.h"
#include "../utils/trackSWF.h"
#include "../utils/errcodes.h"


std::vector<uint8_t> decompressSWF(std::vector<uint8_t>& SWFFile, uint32_t decompressedFileLength, int compressionType, uint32_t SWFCompressedLength, LZMA lzProperties) {

    
    std::vector<uint8_t> binOut(decompressedFileLength);
    
    switch( compressionType ) {

        // CWS - ZLIB Decompression
            case 1:

                SWFShift(SWFFile, 8);

                z_stream stream;
                stream.zalloc = Z_NULL;
                stream.zfree = Z_NULL;
                stream.opaque = Z_NULL;
                stream.avail_in = SWFFile.size();
                stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(SWFFile.data()));
                stream.avail_out = decompressedFileLength;
                stream.next_out = reinterpret_cast<Bytef*>(binOut.data());

                // Errors:
                    switch(inflateInit(&stream)) {
                        
                        // Error Code 41 - ERRNO
                            case -1:
                                throwErr(41);
                            break;

                        // Error Code 42 - STREAM_ERROR
                            case -2:
                                throwErr(42);
                            break;

                        //Error Code 43 - DATA_ERROR
                            case -3:
                                throwErr(43);
                            break;
                        
                        //Error Code 44 - MEM_ERROR
                            case -4:
                                throwErr(44);
                            break;

                        //Error Code 45 - BUF_ERROR
                            case -5:
                                throwErr(45);
                            break;

                        //Error Code 46 - VERSION_ERROR
                            case -6:
                                throwErr(46);
                            break;
                            
                        //Success 
                            case Z_OK:
                                std::cout << "Successful Decompression! Finishing up...\n";
                                inflate(&stream, Z_FINISH);
                                inflateEnd(&stream);
                                return binOut;
                            break;

                        //Else
                            default:
                                if ( stream.msg ) {

                                    std::cerr << "ZLIB Decompression Failed With Error: " << stream.msg << "\n";
                                    exit(1);

                                }
                                else {

                                    std::cerr << "ZLIB Decompression Failed With An Unknown Error!\n This Error Should Never Arise! Please contact program developer(s)!\n";
                                    exit(1000);

                                }

                            break;

                    }

            break;

        // ZWS - LZMA Decompression
            case 2:
                SWFShift(SWFFile, 17);

                lzma_options_lzma options;
                options.lc = lzProperties.LZMAProperties[0];
                options.lp = lzProperties.LZMAProperties[1];
                options.pb = lzProperties.LZMAProperties[2];
                options.dict_size = lzProperties.LZMADictionary;

                lzma_filter filters[2];
                filters[0].id = LZMA_FILTER_LZMA1;
                filters[0].options = &options;
                filters[1].id = LZMA_VLI_UNKNOWN;

                lzma_stream lzstream = LZMA_STREAM_INIT;
                lzstream.next_in = SWFFile.data();
                lzstream.avail_in = SWFCompressedLength;
                lzstream.next_out = reinterpret_cast<uint8_t*>(binOut.data());
                lzstream.avail_out = decompressedFileLength;

                // Errors:
                    switch (lzma_raw_decoder(&lzstream, filters)) {
                    // Error Code 51 - LZMA_MEM_ERROR
                        case LZMA_MEM_ERROR:
                            throwErr(51);
                        break;

                    // Error Code 52 - LZMA_OPTIONS_ERROR
                        case LZMA_OPTIONS_ERROR:
                            throwErr(52);
                        break;

                    // Success - Start Decompression
                        case LZMA_OK:
                            std::cout << "Startup successful! Beginning decompression...\n";
                            switch (lzma_code(&lzstream, LZMA_FINISH)) {
                                // Error Code 53 - LZMA_DATA_ERROR
                                    case LZMA_DATA_ERROR:
                                        lzma_end(&lzstream);
                                        throwErr(53);
                                    break;

                                // Error Code 54 - LZMA_BUF_ERROR
                                    case LZMA_BUF_ERROR:
                                        lzma_end(&lzstream);
                                        throwErr(54);
                                    break;
                                // Error Code 55 - LZMA_MEM_ERROR (during decode)
                                    case LZMA_MEM_ERROR:
                                        lzma_end(&lzstream);
                                        throwErr(55);
                                    break;

                                case LZMA_STREAM_END: // End
                                    case LZMA_OK: // Success
                                        std::cout << "Successful Decompression! Finishing up...\n";
                                        lzma_end(&lzstream);
                                        return binOut;
                                    break;
                                break;

                                default:
                                    std::cerr << "LZMA Decompression Failed With An Unknown Error!\n This Error Should Never Arise! Please contact program developer(s)!\n";
                                    lzma_end(&lzstream);
                                    exit(1000);
                                break;

                            }

                        break;

                        default:
                            std::cerr << "LZMA Decoder Init Failed With An Unknown Error!\n This Error Should Never Arise! Please contact program developer(s)!\n";
                            exit(1000);
                        break;
                    
                    }
    }

}