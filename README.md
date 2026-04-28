# WasFlash 

Currently:
An SWF Movie Player that plays typical SWF video like files, and shape only flash animations, with audio! It works for both Linux, and HTML5.

The Hope:

A (Hopefully) Fully Featured Flash Player Replacement(Say that 10 times fast), designed for both PC and Web(Using EMScripten :D). In essence, this would be able to run flash games in the modern day. This will process .SWF files and display their contents, much like current SWF players like Ruffle and LightSpark, but with a higher focus on portability, and simple integration with websites.

## Goals & Progress

- [x] Finish header parsing
  - [x] LZMA Decompression
  - [x] FrameRate
  - [x] FrameCount
- [ ] Work on low-level tag parsing
  - [x] Parse Tag Header
  - [x] Fully parse a very simple SWF File
- [ ] Work on higher-level tag parsing
- [ ] Work on ActionScript parsing
- [ ] Work on ActionScript interpreter
- [ ] Work on making an ActionScript instruction engine
- [ ] Community testing

Please feel free to use the included swf files in samples if you do not have any!

## License

TBD

---

*WasFlash is not affiliated with Adobe Inc. Flash and SWF are trademarks of Adobe.*
