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

    ### -- ZLIB -- ###

        git clone https://github.com/madler/zlib.git

    ### -- LZMA -- ###

        git clone https://github.com/kobolabs/liblzma

    ### -- End -- ###
    
### --- End --- ###


### --- Building Repos --- ###
    if [debian]; then
    ### -- Linux -- ###

        ### - FFmpeg - ###

            cd FFmpeg
            ./configure --prefix=$PWD/build --disable-shared --enable-static --enable-optimizations --extra-cflags="-march=native -O3" --extra-cxxflags="-march=native -O3" --disable-programs --disable-doc --disable-network
            make
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
            ninja -C out/build skia
            mkdir -p $PWD/../include/skia/lib
            cp $PWD/out/build/* -r $PWD/../include/skia/lib
            cd ../

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
            emconfigure ./configure --disable-asm --disable-x86asm --cc=emcc --cxx=em++ --ar=emar --enable-cross-compile --target-os=none --arch=x86_32 --disable-programs --disable-doc --disable-network --nm=emnm --ranlib=emranlib --prefix=$PWD/build
            emmake make
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
            ninja -C out/build skia
            mkdir -p $PWD/../include/skia/lib
            cp $PWD/out/build/* -r $PWD/../include/skia/lib
            cd ../
            
        ### LZMA
            cd liblzma
            emconfigure ./configure --disable-shared --enable-static --disable-xz --disable-xzdec --disable-lzmadec --disable-lzmainfo --disable-scripts --host=wasm32-unknown-linux --prefix=$PWD/build
            emmake make
            make install
            mkdir -p $PWD/../include/liblzma/include/
            mkdir -p $PWD/../include/liblzma/lib
            cp $PWD/build/include/* -r $PWD/../include/liblzma/include/
            cp $PWD/build/lib/* -r $PWD/../include/liblzma/lib

### --- End --- ###
            