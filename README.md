# WasFlash 

Currently: 
Currently: An SWF Movie Player that plays typical SWF video like files, and shape only flash animations, with audio! It works for both Linux, and HTML5.

The Hope:

A (Hopefully) Fully Featured Flash Player Replacement(Say that 10 times fast), designed for both PC and Web(Using EMScripten :D). In essence, this would be able to run flash games in the modern day. This will process .SWF files and display their contents, much like current SWF players like Ruffle and LightSpark, but with a higher focus on portability, and simple integration with websites.  

## Goals & Progress - Current Scope

- [ ] Full support without ActionScript
    - [x] Gradient Support - Almost Done(Tied to Defineshape4)...
    - [x] DefineShape 4 Support
        - [x] Parse tag
        - [x] Do a little reworking of the shape processor to work with new flags.
        - [x] Add to tag pipeline
        - [x] Ensure rendering can handle new flags
    - [ ] Bitmap Support
        - [ ] DefineBits
        - [ ] JPEGTables
        - [ ] DefineBitsJPEG2
        - [ ] DefineBitsJPEG3
        - [ ] DefineBitsJPEG4
        - [ ] DefineBitsLossless
        - [ ] DefineBitsLossless2
        - [ ] Impliment into DefineShape Tags.
    - [ ] DefineFont Support
    - [ ] DefineText Support
    - [ ] PlaceCharacter3
    - [ ] Optimize for smoother playback
    - [ ] Work on an actual dependency script
    - [ ] To be continued...


Please feel free to use the included swf files in samples if you do not have any!
---

*WasFlash is not affiliated with Adobe Inc. Flash and SWF are trademarks of Adobe.*
