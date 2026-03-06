#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "decomp.h"
#include "zlib.h"
#include "../utils/trackSWF.h"
#include "../utils/errcodes.h"


std::vector<uint8_t> decompressSWF(std::vector<uint8_t>& SWFFile, uint32_t decompressedFileLength, int compressionType) {

    SWFShift(SWFFile, 8);
    
    switch( compressionType ) {

        // CWS - ZLIB Decompression
            case 1:
                
                std::vector<uint8_t> binOut(decompressedFileLength);

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

    }

}