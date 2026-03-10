#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <atomic>
#include "header/header.h"
#include "tags/tags.h"
#include "rendering/renderer.h"
#include "main.h"

int main(int argc, char* argv[]) {

    std::cout << argv[1] << "\n";
    std::vector<uint8_t> inputFile = readFile(argv[1]);
    
    SWFHeader header = getSWFHeader(inputFile, argv[1]);

    std::vector<uint8_t> SWFFile = header.SWFFile; 
    rawSWFTag rawTag;
    SWFTag parsedSWFTag;
    
    //Start Renderer:
    std::deque<rendererInstruction> renderStream;
    std::mutex renderStreamMutex;
    std::condition_variable renderCv;
    std::atomic<bool> running(true);

    std::thread renderThread(render,
    std::ref(header.SWFFrameSize),
    std::ref(renderStream),
    std::ref(renderStreamMutex),
    std::ref(renderCv));


    // This is the main stream for the tags:

    std::deque<SWFTag> stream;
    std::mutex streamMutex;
    std::condition_variable cv;
    bool done = false;
        

    
    std::thread processorThread(processor, 
    std::ref(stream), 
    std::ref(streamMutex), 
    std::ref(cv), 
    std::ref(done),
    std::ref(renderStream),
    std::ref(renderStreamMutex),
    std::ref(renderCv),
    std::ref(running),
    std::cref(header));

    

    for(int x = 0; x < 2; x++) {
        
        rawTag = getSWFTag(SWFFile);
        parsedSWFTag = parseSWFTag(rawTag);
        pushTag(parsedSWFTag, stream, streamMutex, cv);
    }
    
    {
        std::lock_guard<std::mutex> lock(streamMutex);
        done = true;
    }
    renderCv.notify_one();
    cv.notify_one();
    processorThread.join();

    renderThread.join();
    
}