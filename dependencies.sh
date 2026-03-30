### Dependency Builder ###
### ------------------ ###

if [ -f /etc/os-release ]; then
    . /etc/os-release
    
    
    if [ "$ID" = "debian" ] || [[ "$ID_LIKE" =~ "debian" ]]; then
        sudo apt update && sudo apt install -y nasm libglfw3-dev ninja-build gcc make autoconf automake libtool libglu1-mesa-dev
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

        git clone /home/silverhoft/Documents/Projects/Flash For Wasm/swfParse

    ### -- End -- ###
    
### --- End --- ###

### --- Building Repos --- ###

    ### -- Linux -- ###

        ### - FFmpeg - ###

            cd FFmpeg
            ./configure --prefix=$PWD/build
            make
            make install
            mkdir -p $PWD/../include/FFmpeg/include/
            mkdir -p $PWD/../include/FFmpeg/lib
            cp $PWD/build/include/* -r $PWD/../include/FFmpeg/include/
            cp $PWD/build/lib/* -r $PWD/../include/FFmpeg/lib

        ### - Skia - ###

            cd skia
            python3 tools/git-sync-deps
            python3 bin/fetch-ninja
            bin/gn gen out/build --args="
            is_debug = false
            is_official_build = true

            skia_use_gl = true
            skia_enable_ganesh = true

            # Disable everything you don't need
            skia_enable_skottie = false
            skia_enable_skparagraph = false
            skia_enable_svg = false
            skia_enable_skshaper = false
            skia_use_icu = false
            skia_use_harfbuzz = false
            skia_use_expat = false
            skia_use_libjpeg_turbo_decode = false
            skia_use_libjpeg_turbo_encode = false
            skia_use_libwebp_decode = false
            skia_use_libwebp_encode = false
            skia_use_zlib = false
            skia_use_wuffs = false
            skia_enable_pdf = false
            skia_use_freetype = false
            skia_use_fontconfig = false
            "
            ninja -C out/build
            mkdir -p $PWD/../include/skia/lib
            cp $PWD/out/build/* -r $PWD/../include/skia/lib

        ### - End - ###


### --- End --- ###
            