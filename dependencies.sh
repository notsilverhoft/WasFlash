### Dependency Builder ###
### ------------------ ###

if [ -f /etc/os-release ]; then
    . /etc/os-release
    
    
    if [ "$ID" = "debian" ] || [[ "$ID_LIKE" =~ "debian" ]]; then
        sudo apt update && sudo apt install -y nasm libglfw3-dev ninja-build gcc make autoconf automake libtool libglu1-mesa-dev
        export debian = true
    fi
else
    echo "/etc/os-release not found, falling back to other methods..."
    # Fallback methods can be placed here
fi

### --- Pulling Repos --- ###


    ### -- FFMPEG -- ### 

        git clone https://github.com/FFmpeg/FFmpeg

    ### -- Skia -- ###

        git clone https://skia.googlesource.com/skia.git  
        cd skia
        git checkout cecc0e0da9ae2f06ea1e382234a69ad42408fd88
        cd ../

    ### -- LZMA -- ###

        git clone https://github.com/kobolabs/liblzma

    ### -- LibJpegTurbo -- ###

        wget https://github.com/libjpeg-turbo/libjpeg-turbo/archive/refs/tags/3.1.4.1.zip
        unzip 3.1.4.1.zip
        rm 3.1.4.1.zip

    ### -- MiniAudio -- ###

        wget https://raw.githubusercontent.com/mackron/miniaudio/refs/heads/master/miniaudio.h


    ### -- Wuffs -- ##

        wget https://raw.githubusercontent.com/google/wuffs/refs/heads/main/release/c/wuffs-v0.4.c # Plan to use for full zlib support.

    ### -- End -- ###
    
### --- End --- ###


### --- Building Repos --- ###

    ### -- All -- ###

        mkdir -p $PWD/include/MiniAudio/
        mkdir -p $PWD/include/wuffs/
        mv miniaudio.h include/MiniAudio/miniaudio.h
        mv wuffs-v0.4.c include/wuffs/wuffs-v0.4.c
        
    ### -- Linux -- ###

        ### - FFmpeg - ###

            cd FFmpeg
            ./configure --prefix=$PWD/build --disable-shared --enable-static --enable-optimizations --extra-cflags="-march=native -O3 -funroll-loops" --extra-cxxflags="-march=native -O3 -funroll-loops" --disable-programs --disable-doc --disable-network
            make -j$(nproc)
            make install
            mkdir -p $PWD/../include/FFmpeg/include/
            mkdir -p $PWD/../include/FFmpeg/lib
            cp $PWD/build/include/* -r $PWD/../include/FFmpeg/include/
            cp $PWD/build/lib/* -r $PWD/../include/FFmpeg/lib
            cd ../

        ### - Skia - ###

            cd skia
            python3 tools/git-sync-deps
            python3 bin/fetch-ninja
            mkdir -p $PWD/out/build
            bin/gn gen out/build --args='is_official_build=true extra_cflags=["-O3", "-march=native", "-funroll-loops"] extra_cflags_cc=["-O3", "-march=native", "-funroll-loops"] skia_use_libjpeg_turbo_decode = false skia_use_libjpeg_turbo_encode = false' # This is assuming you are using optimizations. This will change...
            ninja -C out/build skia -j$(nproc)
            mkdir -p $PWD/../include/skia/lib
            cp $PWD/out/build/* -r $PWD/../include/skia/lib
            cd ../

        ### - LibJpegTurbo - ###

            cd libjpeg-turbo-3.1.4.1
            mkdir -p build && cd build
            cmake -G"Unix Makefiles" -DCMAKE_C_FLAGS="-march=native -O3 -funroll-loops" --install-prefix $PWD/build ../
            make -j$(nproc)
            make install
            mkdir -p $PWD/../../include/libJPEGTurbo/include
            cp $PWD/build/include/* -r $PWD/../../include/libJPEGTurbo/include/
            mkdir -p $PWD/../../include/libJPEGTurbo/lib
            cp $PWD/build/lib/* -r $PWD/../../include/libJPEGTurbo/lib/
            cd ../../

        ### - End - ###
    fi

    ### -- Emscripten -- ##

        git clone https://github.com/emscripten-core/emsdk
        cd emsdk
        ./emsdk install latest
        ./emsdk activate latest
        source emsdk_env.sh
        embuilder build zlib
        cd ../

        ### - FFmpeg - ###

            cd FFmpeg
            emconfigure ./configure --disable-asm --disable-x86asm --cc=emcc --cxx=em++ --extra-cflags="-msimd128 -O3 -funroll-loops" --extra-cxxflags="-msimd128 -O3 -funroll-loops" --ar=emar --enable-cross-compile --target-os=none --arch=x86_32 --disable-programs --disable-doc --disable-network --nm=emnm --ranlib=emranlib --prefix=$PWD/build
            emmake make -j$(nproc)
            make install
            mkdir -p $PWD/../include/FFmpeg/include/
            mkdir -p $PWD/../include/FFmpeg/lib
            cp $PWD/build/include/* -r $PWD/../include/FFmpeg/include/
            cp $PWD/build/lib/* -r $PWD/../include/FFmpeg/lib
            cd ../

        ### - Skia - ###

            cd skia
            python3 tools/git-sync-deps
            python3 bin/fetch-ninja
            mkdir -p $PWD/out/build
            cp $PWD/../EMScriptenArgs.gn $PWD/out/build/args.gn
            bin/gn gen out/build 
            ninja -C out/build skia -j$(nproc)
            mkdir -p $PWD/../include/skia/lib
            cp $PWD/out/build/* -r $PWD/../include/skia/lib
            cd ../
            
        ### LZMA

            cd liblzma
            emconfigure ./configure --disable-shared --enable-static --disable-xz --disable-xzdec --disable-lzmadec --disable-lzmainfo --disable-scripts --host=wasm32-unknown-linux --prefix=$PWD/build CFLAGS="-msimd128 -O3 -funroll-loops" CXXFLAGS="-msimd128 -O3 -funroll-loops"
            emmake make -j$(nproc)
            make install
            mkdir -p $PWD/../include/liblzma/include/
            mkdir -p $PWD/../include/liblzma/lib
            cp $PWD/build/include/* -r $PWD/../include/liblzma/include/
            cp $PWD/build/lib/* -r $PWD/../include/liblzma/lib
            cd ../
        
        ### LibJpegTurbo

            cd libjpeg-turbo-3.1.4.1
            mkdir -p build && cd build
            emcmake cmake -G"Unix Makefiles" -DCMAKE_C_FLAGS="-O3 -funroll-loops" -DWITH_SIMD=OFF -DENABLE_SHARED=OFF -DENABLE_STATIC=ON --install-prefix $PWD/build ../
            emmake make -j$(nproc)
            make install
            mkdir -p $PWD/../../include/libJPEGTurbo/include
            cp $PWD/build/include/* -r $PWD/../../include/libJPEGTurbo/include/
            mkdir -p $PWD/../../include/libJPEGTurbo/lib
            cp $PWD/build/lib/* -r $PWD/../../include/libJPEGTurbo/lib/
            cd ../../

        ### -- End -- ###

### --- End --- ###
            