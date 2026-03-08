# WaFlash ⚡

A (Hopefully) Fully Featured Flash Player Replacement, designed for both PC and Web (Using Emscripten :D). This will process .SWF files and display their contents, much like current SWF players like Ruffle and LightSpark, but with a higher focus on portability, and simple integration with websites.

---

## What is Flash Player?
Flash Player was an old plugin for chrome browsers that would allow users to run SWF files directly off of websites. This was used for games, videos, and even physics simulations. Some of my favorite examples, were the Papa's Games. In fact, Papa's Scooperia is my entire motivation to create this, as I used to play it as a kid. The point is, people loved the games that came out of Flash, and when Adobe discontinued the plugin, many of us were devistated.

## Goals & Progress

- [x] Finish header parsing
  - [x] LZMA Decompression
  - [x] FrameRate
  - [x] FrameCount
- [ ] Work on low-level tag parsing
  - [x] Parse Tag Header
  - [ ] Fully parse a very simple SWF File
    - [ ] Tag #69 - FileAttributes Tag
- [ ] Work on higher-level tag parsing
- [ ] Work on low-level tag interpretation
- [ ] Work on rendering low-level info
- [ ] Work on higher-level tag interpretation
- [ ] Work on rendering higher-level info
- [ ] Work on ActionScript parsing
- [ ] Work on ActionScript interpreter
- [ ] Work on making an ActionScript instruction engine
- [ ] Implement rendering with instruction
- [ ] Community testing



## License

TBD

---

*WaFlash is not affiliated with Adobe Inc. Flash and SWF are trademarks of Adobe.*
