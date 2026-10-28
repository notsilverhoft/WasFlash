#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags/tags.h"
#include "../utils/trackSWF.h"
#include "spriteProcessor.h"

Sprite getSprite(SWFTag rawTag) {

    Sprite binOut;

    binOut.SpriteID = rawTag.DefineSprite.SpriteID;

    int f = 0;
    int t = 0;
    bool End = false;

    while (true) {



        if ( End ) {

            break;

        }

        int i = 0;

        if ( binOut.Frames.size() <= f ) {

            binOut.Frames.resize(f+1);
            
        }

        while (true) {


            if ( rawTag.DefineSprite.ControlTags[t].tagCode == 1 ) {


                // std::cout << "Sprite: ControlTag: Tag ID: " << rawTag.DefineSprite.ControlTags[t].tagCode << ': "ShowFrame"';

                f++;
                i = 0;
                t++;
                break;

            }

            else if ( rawTag.DefineSprite.ControlTags[t].tagCode == 4 ) {

                binOut.Frames[f].FrameInstructions.resize(i+1);

                // std::cout << "Sprite: ControlTag: Tag ID: " << rawTag.DefineSprite.ControlTags[t].tagCode << ': "PlaceObject"';

                binOut.Frames[f].FrameInstructions[i].InstructionType = 1;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasMatrix = true;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasColorTransform;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasRatio = false;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasName = false;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasClipDepth;
                // Clip Actions = false;

                binOut.Frames[f].FrameInstructions[i].PlaceObject.CharacterID = rawTag.DefineSprite.ControlTags[t].PlaceObject.CharacterID;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.Depth = rawTag.DefineSprite.ControlTags[t].PlaceObject.Depth;
                binOut.Frames[f].FrameInstructions[i].PlaceObject.Matrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.Matrix;
            

                if ( binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasColorTransform ) {

                    binOut.Frames[f].FrameInstructions[i].PlaceObject.ColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.ColorTransform;

                }

                t++;
                i++; 

            }

            else if ( rawTag.DefineSprite.ControlTags[t].tagCode == 26 ) {

                binOut.Frames[f].FrameInstructions.resize(i+1);

                // std::cout << "Sprite: ControlTag: Tag ID: " << rawTag.DefineSprite.ControlTags[t].tagCode << ': "PlaceObject2"';

                if ( rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasCharacter && !rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagMove ) { // Place

                    binOut.Frames[f].FrameInstructions[i].InstructionType = 1;
                    binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasMatrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasMatrix;
                    binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasColorTransform;
                    binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasRatio = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasRatio;
                    binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasName = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasName;
                    binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasClipDepth;
                    // Clip Actions;

                    binOut.Frames[f].FrameInstructions[i].PlaceObject.CharacterID = rawTag.DefineSprite.ControlTags[t].PlaceObject.CharacterID;
                    binOut.Frames[f].FrameInstructions[i].PlaceObject.Depth = rawTag.DefineSprite.ControlTags[t].PlaceObject.Depth;

                    if ( binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasMatrix ) {

                        binOut.Frames[f].FrameInstructions[i].PlaceObject.Matrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.Matrix;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasColorTransform ) {

                        binOut.Frames[f].FrameInstructions[i].PlaceObject.ColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.ColorTransform;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasRatio ) {

                        binOut.Frames[f].FrameInstructions[i].PlaceObject.Ratio = rawTag.DefineSprite.ControlTags[t].PlaceObject.Ratio;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasName ) {

                        binOut.Frames[f].FrameInstructions[i].PlaceObject.Name = rawTag.DefineSprite.ControlTags[t].PlaceObject.Name;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].PlaceObject.PlaceFlagHasClipDepth ) {

                        binOut.Frames[f].FrameInstructions[i].PlaceObject.ClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.ClipDepth;

                    }

                    // Clip Actions

                    t++;
                    i++;

                }

                else if ( !rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasCharacter && rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagMove ) { // Modify

                    binOut.Frames[f].FrameInstructions[i].InstructionType = 2;
                    binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasMatrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasMatrix;
                    binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasColorTransform;
                    binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasRatio = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasRatio;
                    binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasName = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasName;
                    binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasClipDepth;

                    binOut.Frames[f].FrameInstructions[i].ModifyObject.Depth = rawTag.DefineSprite.ControlTags[t].PlaceObject.Depth;

                    if ( binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasMatrix ) {

                        binOut.Frames[f].FrameInstructions[i].ModifyObject.Matrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.Matrix;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasColorTransform ) {

                        binOut.Frames[f].FrameInstructions[i].ModifyObject.ColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.ColorTransform;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasRatio ) {

                        binOut.Frames[f].FrameInstructions[i].ModifyObject.Ratio = rawTag.DefineSprite.ControlTags[t].PlaceObject.Ratio;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasName ) {

                        binOut.Frames[f].FrameInstructions[i].ModifyObject.Name = rawTag.DefineSprite.ControlTags[t].PlaceObject.Name;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ModifyObject.PlaceFlagHasClipDepth ) {

                        binOut.Frames[f].FrameInstructions[i].ModifyObject.ClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.ClipDepth;

                    }

                    t++;
                    i++;

                }

                else if ( rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasCharacter && rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagMove ) { // Replace

                    binOut.Frames[f].FrameInstructions[i].InstructionType = 3;
                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasMatrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasMatrix;
                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasColorTransform;
                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasRatio = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasRatio;
                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasName = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasName;
                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.PlaceFlagHasClipDepth;
                    // Clip Actions;

                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.CharacterID = rawTag.DefineSprite.ControlTags[t].PlaceObject.CharacterID;
                    binOut.Frames[f].FrameInstructions[i].ReplaceObject.Depth = rawTag.DefineSprite.ControlTags[t].PlaceObject.Depth;

                    if ( binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasMatrix ) {

                        binOut.Frames[f].FrameInstructions[i].ReplaceObject.Matrix = rawTag.DefineSprite.ControlTags[t].PlaceObject.Matrix;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasColorTransform ) {

                        binOut.Frames[f].FrameInstructions[i].ReplaceObject.ColorTransform = rawTag.DefineSprite.ControlTags[t].PlaceObject.ColorTransform;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasRatio ) {

                        binOut.Frames[f].FrameInstructions[i].ReplaceObject.Ratio = rawTag.DefineSprite.ControlTags[t].PlaceObject.Ratio;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasName ) {

                        binOut.Frames[f].FrameInstructions[i].ReplaceObject.Name = rawTag.DefineSprite.ControlTags[t].PlaceObject.Name;

                    }

                    if ( binOut.Frames[f].FrameInstructions[i].ReplaceObject.PlaceFlagHasClipDepth ) {

                        binOut.Frames[f].FrameInstructions[i].ReplaceObject.ClipDepth = rawTag.DefineSprite.ControlTags[t].PlaceObject.ClipDepth;

                    }

                    // Clip Actions

                    t++;
                    i++;

                }

                else {

                    t++;

                }

            }

            else if ( rawTag.DefineSprite.ControlTags[t].tagCode == 5 || rawTag.DefineSprite.ControlTags[t].tagCode == 28 ) {

                binOut.Frames[f].FrameInstructions.resize(i+1);

                // std::cout << "Sprite: ControlTag: Tag ID: " << rawTag.DefineSprite.ControlTags[t].tagCode << ': "RemoveObject"';

                binOut.Frames[f].FrameInstructions[i].InstructionType = 4;

                binOut.Frames[f].FrameInstructions[i].RemoveObject.Depth = rawTag.DefineSprite.ControlTags[t].RemoveObject.Depth;

                t++;
                i++;
            
            }

            else if ( rawTag.DefineSprite.ControlTags[t].tagCode == 0 ) {

                End = true;
                break;

            }

            else {

                t++;

            }
        }

    }

    if ( f > 0 ) {

        binOut.Frames.resize(f);

    }

    return binOut;

}

ParsedFrame GetParsedSpriteFrame(Sprite sprite, std::unordered_map<uint16_t, SWFCharacter>& CharacterList, bool IsInitial, ParsedFrame CurrentFrame) {

    ParsedFrame binOut;

    if ( IsInitial ) {

        binOut.FrameNumber = 0;

    }

    else {

        binOut.FrameNumber = CurrentFrame.FrameNumber + 1;

    }

    if ( binOut.FrameNumber >= sprite.Frames.size() ) {

        binOut.FrameNumber = 0;

    }

    if ( IsInitial || binOut.FrameNumber == 0 ) {

        binOut.depthList = {};

    }

    else {

        binOut.depthList = CurrentFrame.depthList;

    }

    for ( int i = 0; i < sprite.Frames[binOut.FrameNumber].FrameInstructions.size(); i++ ) {

        FrameInstruction currentInstruction = sprite.Frames[binOut.FrameNumber].FrameInstructions[i];

        if ( currentInstruction.InstructionType == 1 ) { // PlaceObject

            if ( binOut.depthList.find(currentInstruction.PlaceObject.Depth) == binOut.depthList.end() ) {

                if ( CharacterList[currentInstruction.PlaceObject.CharacterID].code == 2 || CharacterList[currentInstruction.PlaceObject.CharacterID].code == 3 || CharacterList[currentInstruction.PlaceObject.CharacterID].code == 4 ) { // DefineShape 1,2,3,4 + DefineSprite + DefineMorphShape

                    binOut.depthList[currentInstruction.PlaceObject.Depth] = buildInstruction(currentInstruction.PlaceObject.CharacterID, currentInstruction.PlaceObject.Matrix, true, currentInstruction.PlaceObject.ColorTransform, currentInstruction.PlaceObject.Ratio);

                    if ( currentInstruction.PlaceObject.PlaceFlagHasClipDepth ) {

                        binOut.depthList[currentInstruction.PlaceObject.Depth].LocalHasClipDepth = true;
                        binOut.depthList[currentInstruction.PlaceObject.Depth].LocalClipDepth = currentInstruction.PlaceObject.ClipDepth;

                    }

                }

                else if ( CharacterList[currentInstruction.PlaceObject.CharacterID].code == 0 ) {

                    continue;

                }

                else {

                    std::cout << "Warning! Sprite: Place: Skipping Tag: Tag Type: " << CharacterList[currentInstruction.PlaceObject.CharacterID].code << ": Not Yet Implimented In Sprites!" << "\n";
                    
                }

            }

            else { // If for some reason a Place Command is called on an occupied Depth

                std::cout << "Warning! Sprite: Place: Skipping Tag: Depth Is Already Full!" << "\n";

            }

        }

        
        
        if ( currentInstruction.InstructionType == 2 ) { // ModifyObject

            int type;

            auto entry = binOut.depthList.find(currentInstruction.ModifyObject.Depth);

            if ( entry != binOut.depthList.end() ) {

                type = entry->second.instructionCode;

                if ( type == 4 ) {

                    if ( currentInstruction.ModifyObject.PlaceFlagHasMatrix && !currentInstruction.ModifyObject.PlaceFlagHasColorTransform && !currentInstruction.ModifyObject.PlaceFlagHasRatio ) {

                        binOut.depthList[currentInstruction.ModifyObject.Depth].LocalMatrix = currentInstruction.ModifyObject.Matrix;

                    }

                    else {

                        MATRIX StandingMatrix = binOut.depthList[currentInstruction.ModifyObject.Depth].LocalMatrix;
                        bool StandingHasColorTransform = binOut.depthList[currentInstruction.ModifyObject.Depth].LocalHasColorTransform;
                        CXFORMWITHALPHA StandingColorTransform = binOut.depthList[currentInstruction.ModifyObject.Depth].LocalColorTransform;
                        uint16_t StandingRatio = binOut.depthList[currentInstruction.ModifyObject.Depth].LocalRatio;
                        bool StandingHasClipDepth = binOut.depthList[currentInstruction.ModifyObject.Depth].LocalHasClipDepth;
                        uint16_t StandingClipDepth = binOut.depthList[currentInstruction.ModifyObject.Depth].LocalClipDepth;
                        
                        if ( currentInstruction.ModifyObject.PlaceFlagHasMatrix ) {

                            StandingMatrix = currentInstruction.ModifyObject.Matrix;

                        }

                        if ( currentInstruction.ModifyObject.PlaceFlagHasColorTransform ) {

                            StandingHasColorTransform = true;
                            StandingColorTransform = currentInstruction.ModifyObject.ColorTransform;

                        }

                        if ( currentInstruction.ModifyObject.PlaceFlagHasRatio ) {

                            StandingRatio = currentInstruction.ModifyObject.Ratio;

                        }

                        binOut.depthList[currentInstruction.ModifyObject.Depth] = buildInstruction(binOut.depthList[currentInstruction.ModifyObject.Depth].LocalCharacterID, StandingMatrix, StandingHasColorTransform, StandingColorTransform, StandingRatio);

                        binOut.depthList[currentInstruction.ModifyObject.Depth].LocalHasClipDepth = StandingHasClipDepth;
                        binOut.depthList[currentInstruction.ModifyObject.Depth].LocalClipDepth = StandingClipDepth;

                    }

                }

                else if ( type == 3 || type == 5 ) {

                    if ( currentInstruction.ModifyObject.PlaceFlagHasMatrix ) {

                        binOut.depthList[currentInstruction.ModifyObject.Depth].LocalMatrix = currentInstruction.ModifyObject.Matrix;

                    }

                    if ( currentInstruction.ModifyObject.PlaceFlagHasColorTransform ) {

                        binOut.depthList[currentInstruction.ModifyObject.Depth].LocalHasColorTransform = true;
                        binOut.depthList[currentInstruction.ModifyObject.Depth].LocalColorTransform = currentInstruction.ModifyObject.ColorTransform;

                    }

                }

                else {

                    std::cout << "Warning! Sprite: Modify: Skipping Tag: Tag Type: Item At Depth Is Not Currently Supported!" << "\n";

                }

            }

        }

        if ( currentInstruction.InstructionType == 3 ) { // ReplaceObject

            MATRIX replaceMatrix = currentInstruction.ReplaceObject.Matrix;

            if ( !currentInstruction.ReplaceObject.PlaceFlagHasMatrix ) { // No matrix, keep the matrix of what is at this depth

                auto oldEntry = binOut.depthList.find(currentInstruction.ReplaceObject.Depth);

                if ( oldEntry != binOut.depthList.end() ) {

                    replaceMatrix = oldEntry->second.LocalMatrix;

                }

            }

            binOut.depthList.erase(currentInstruction.ReplaceObject.Depth);

            if ( CharacterList[currentInstruction.ReplaceObject.CharacterID].code == 2 || CharacterList[currentInstruction.ReplaceObject.CharacterID].code == 3 || CharacterList[currentInstruction.ReplaceObject.CharacterID].code == 4 ) { // DefineShape 1,2,3,4 + DefineSprite + DefineMorphShape

                binOut.depthList[currentInstruction.ReplaceObject.Depth] = buildInstruction(currentInstruction.ReplaceObject.CharacterID, replaceMatrix, true, currentInstruction.ReplaceObject.ColorTransform, currentInstruction.ReplaceObject.Ratio);

                if ( currentInstruction.ReplaceObject.PlaceFlagHasClipDepth ) {

                    binOut.depthList[currentInstruction.ReplaceObject.Depth].LocalHasClipDepth = true;
                    binOut.depthList[currentInstruction.ReplaceObject.Depth].LocalClipDepth = currentInstruction.ReplaceObject.ClipDepth;

                }

            }

            else if ( CharacterList[currentInstruction.ReplaceObject.CharacterID].code == 0 ) {

                continue;

            }

            else {

                std::cout << "Warning! Sprite: Replace: Skipping Tag: Tag Type: " << CharacterList[currentInstruction.ReplaceObject.CharacterID].code << ": Not Yet Implimented In Sprites!" << "\n";

            }

        }

        if ( currentInstruction.InstructionType == 4 ) { // RemoveObject

            binOut.depthList.erase(currentInstruction.RemoveObject.Depth);

        }

    }

    return binOut;

}