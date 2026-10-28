#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "../base/matrix.h"
#include "../base/colorTransformAlpha.h"
#include "renderer.h"

struct SWFCharacter;
struct SWFTag;

struct SWFCharacter;

struct FrameInstruction {

int InstructionType; // 1 - Place Object   2 - Modify Object   3 - Replace Object  4 - Remove Object

struct { // 1 - Place Object -- Initial placement of a new object at a specified depth.

bool PlaceFlagHasMatrix;
bool PlaceFlagHasColorTransform;
bool PlaceFlagHasRatio;
bool PlaceFlagHasName;
bool PlaceFlagHasClipDepth;
uint16_t CharacterID;
uint16_t Depth;
MATRIX Matrix = {};
CXFORMWITHALPHA ColorTransform = {};
uint16_t Ratio = 0;
std::string Name;
uint16_t ClipDepth;
        // Clip Actions;

    } PlaceObject;

struct { // 2 - Modify Object -- Modify parameters of an existing object at a specified depth.

bool PlaceFlagHasMatrix;
bool PlaceFlagHasColorTransform;
bool PlaceFlagHasRatio;
bool PlaceFlagHasName;
bool PlaceFlagHasClipDepth;
uint16_t Depth;
MATRIX Matrix;
CXFORMWITHALPHA ColorTransform;
uint16_t Ratio;
std::string Name;
uint16_t ClipDepth;

    } ModifyObject;

struct { // 3 - Replace Object -- Replaces current object at a specified depth with a new object.

bool PlaceFlagHasMatrix;
bool PlaceFlagHasColorTransform;
bool PlaceFlagHasRatio;
bool PlaceFlagHasName;
bool PlaceFlagHasClipDepth;
uint16_t CharacterID;
uint16_t Depth;
MATRIX Matrix;
CXFORMWITHALPHA ColorTransform;
uint16_t Ratio;
std::string Name;
uint16_t ClipDepth;
        // Clip Actions;

    } ReplaceObject;

struct { // 4 - Remove Object -- Removes an object from a specified depth.

uint16_t Depth;

    } RemoveObject;


};

struct SpriteFrame {

std::string FrameLabel;
std::vector<FrameInstruction> FrameInstructions;

};

struct Sprite {

uint16_t SpriteID;
std::vector<SpriteFrame> Frames;

};


struct ParsedFrame {

int FrameNumber = 0;
std::map<uint16_t, rendererInstruction> depthList;

};

Sprite getSprite(SWFTag rawTag);
ParsedFrame GetParsedSpriteFrame(Sprite sprite, std::unordered_map<uint16_t, SWFCharacter>& CharacterList, bool IsInitial, ParsedFrame CurrentFrame);