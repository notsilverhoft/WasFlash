### Dependency Builder ###
### ------------------ ###

### --- Pulling Repos --- ###

    ### -- ZLIB -- ###

        git clone https://github.com/madler/zlib.git

    ### -- End -- ###
    
### --- End --- ###

### --- Building Repos --- ###

    ### -- ZLIB -- ###

        cd zlib

        ### - Linux - ###

            ./configure --prefix=./build/
            make
            make install
        
        ### - End - ###
        
    ### -- End -- ###
    
### --- End --- ###
            