#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include "doABC.h"
#include "../../../tags.h"
#include "../../../../utils/trackSWF.h"

uint30_t getVariableUint30(rawSWFTag &rawTag)
{

    uint32_t storage = 0;
    uint8_t currentByte;
    bool running = true;
    int i = 0;

    while (running)
    {

        currentByte = rawTag.tagData[0];
        if (i < 4)
        {

            storage = (((currentByte & 0x7F) << ((i * 7))) | storage);
        }
        else
        {

            storage = (((currentByte & 0x3) << 28) | storage);
        }
        running = (currentByte >> 7);

        SWFShift(rawTag.tagData, 1);

        i++;
    }

    return static_cast<uint30_t>(storage);
}

int32_t getVariableInt32(rawSWFTag &rawTag)
{

    uint32_t storage = 0;
    uint8_t currentByte;
    bool running = true;
    int i = 0;

    while (running)
    {

        currentByte = rawTag.tagData[0];
        if (i < 4)
        {

            storage = (((currentByte & 0x7F) << ((i * 7))) | storage);
        }
        else
        {

            storage = (((currentByte & 0xF) << ((i * 7)) - 3) | storage);
        }
        running = (currentByte >> 7);

        SWFShift(rawTag.tagData, 1);

        i++;
    }

    return static_cast<int32_t>(storage);
}

uint32_t getVariableUint32(rawSWFTag &rawTag)
{

    uint32_t storage = 0;
    uint8_t currentByte;
    bool running = true;
    int i = 0;

    while (running)
    {

        currentByte = rawTag.tagData[0];
        if (i < 4)
        {

            storage = (((currentByte & 0x7F) << ((i * 7))) | storage);
        }
        else
        {

            storage = (((currentByte & 0xF) << ((i * 7)) - 3) | storage);
        }
        running = (currentByte >> 7);

        SWFShift(rawTag.tagData, 1);

        i++;
    }

    return static_cast<uint32_t>(storage);
}

std::vector<int32_t> getIntegers(uint32_t intCount, rawSWFTag &rawTag)
{

    std::vector<int32_t> binOut(intCount + 1);

    for (uint32_t i = 0; i < intCount; i++)
    {

        binOut[i + 1] = getVariableInt32(rawTag);
        // std::cout << "DoABC: ABC File: Constant Pool: Integer Number #" << i + 1 << ": " << (uint32_t)binOut[i + 1] << "\n";
    }

    return binOut;
}

std::vector<uint32_t> getUnsignedIntegers(uint32_t uintCount, rawSWFTag &rawTag)
{

    std::vector<uint32_t> binOut(uintCount + 1);

    for (uint32_t i = 0; i < uintCount; i++)
    {

        binOut[i + 1] = getVariableUint32(rawTag);
        // std::cout << "DoABC: ABC File: Constant Pool: Unsigned Integer Number #" << i + 1 << ": " << (uint32_t)binOut[i + 1] << "\n";
    }

    return binOut;
}

std::vector<double> getDoubles(uint32_t doubleCount, rawSWFTag &rawTag)
{

    std::vector<double> binOut(doubleCount + 1);

    for (uint32_t i = 0; i < doubleCount; i++)
    {

        double result;
        uint8_t *dst = (uint8_t *)&result;
        dst[0] = rawTag.tagData[0];
        dst[1] = rawTag.tagData[1];
        dst[2] = rawTag.tagData[2];
        dst[3] = rawTag.tagData[3];
        dst[4] = rawTag.tagData[4];
        dst[5] = rawTag.tagData[5];
        dst[6] = rawTag.tagData[6];
        dst[7] = rawTag.tagData[7];
        binOut[i + 1] = result;
        SWFShift(rawTag.tagData, 8);
        // std::cout << "DoABC: ABC File: Constant Pool: Double Number #" << i + 1 << ": " << binOut[i + 1] << "\n";
    }

    return binOut;
}

std::vector<uint8_t> getUtf8(uint32_t size, rawSWFTag& rawTag)
{

    std::vector<uint8_t> binOut(size);

    for (uint32_t i = 0; i < size; i++)
    {

        binOut[i] = rawTag.tagData[0];
        SWFShift(rawTag.tagData, 1);
    }

    return binOut;
}

std::vector<stringInfo> getStrings(uint32_t stringCount, rawSWFTag& rawTag)
{

    std::vector<stringInfo> binOut(stringCount + 1);

    for (uint32_t i = 0; i < stringCount; i++) {

        uint32_t size = static_cast<uint32_t>(getVariableUint30(rawTag));
        std::vector<uint8_t> utf8(size + 1);
        utf8 = getUtf8(size, rawTag);
        

        binOut[i + 1] = {size, utf8};

        // std::cout << "DoABC: ABC File: Constant Pool: String: #" << i + 1 << ": Size: " << (uint32_t)binOut[i + 1].size << "\n";
        // std::string str(reinterpret_cast<char *>(binOut[i + 1].utf8.data()), binOut[i + 1].utf8.size());
        // std::cout << "DoABC: ABC File: Constant Pool: String: #" << i + 1 << ": Value: " << str << "\n";
    }

    return binOut;
}

std::vector<namespaceInfo> getNamespaces(uint32_t namespaceCount, rawSWFTag& rawTag) {

    std::vector<namespaceInfo> binOut(namespaceCount + 1);
    
    std::cout << "raw byte: 0x" << std::hex << (int)rawTag.tagData[0] << "\n";

    for (uint32_t i = 0; i < namespaceCount; i++) {

        binOut[i + 1].kind = rawTag.tagData[0];
        SWFShift(rawTag.tagData, 1);

        binOut[i + 1].name = getVariableUint30(rawTag);

        std::cout << "DoABC: ABC File: Constant Pool: Namespace: #" << i + 1 << ": Kind: 0x" << std::hex << std::setfill('0') << std::uppercase << static_cast<uint>(binOut[i + 1].kind) << "\n";
        std::cout << "DoABC: ABC File: Constant Pool: Namespace: #" << i + 1 << ": Name: " << (uint32_t)binOut[i + 1].name << "\n";

    }
    return binOut;
}

SWFTag getDoABCTag(rawSWFTag &rawTag)
{

    SWFTag binOut;
    binOut.tagCode = rawTag.tagCode;

    binOut.DoAbc.parsedABC.minorVersion = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    SWFShift(rawTag.tagData, 2);
    // std::cout << "DoABC: ABC File: Minor Version: " <<  (int)binOut.parsedABC.minorVersion << "\n";

    binOut.DoAbc.parsedABC.majorVersion = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    SWFShift(rawTag.tagData, 2);
    // std::cout << "DoABC: ABC File: Major Version: " <<  (int)binOut.parsedABC.majorVersion << "\n";

    binOut.DoAbc.parsedABC.constantPool.intCount = getVariableUint30(rawTag);
    // std::cout << "DoABC: ABC File: Constant Pool: Integer Count: " << static_cast<int>(binOut.parsedABC.constantPool.intCount) << "\n";

    binOut.DoAbc.parsedABC.constantPool.integers.resize((uint32_t)binOut.DoAbc.parsedABC.constantPool.intCount + 1);
    binOut.DoAbc.parsedABC.constantPool.integers = getIntegers((uint32_t)binOut.DoAbc.parsedABC.constantPool.intCount, rawTag);

    binOut.DoAbc.parsedABC.constantPool.uintCount = getVariableUint30(rawTag);
    // std::cout << "DoABC: ABC File: Constant Pool: Unsigned Integer Count: " << static_cast<int>(binOut.parsedABC.constantPool.uintCount) << "\n";

    binOut.DoAbc.parsedABC.constantPool.uintegers.resize((uint32_t)binOut.DoAbc.parsedABC.constantPool.uintCount + 1);
    binOut.DoAbc.parsedABC.constantPool.uintegers = getUnsignedIntegers((uint32_t)binOut.DoAbc.parsedABC.constantPool.uintCount, rawTag);

    binOut.DoAbc.parsedABC.constantPool.doubleCount = getVariableUint30(rawTag);
    // std::cout << "DoABC: ABC File: Constant Pool: Double Count: " << static_cast<int>(binOut.parsedABC.constantPool.doubleCount) << "\n";

    binOut.DoAbc.parsedABC.constantPool.doubles.resize((uint32_t)binOut.DoAbc.parsedABC.constantPool.doubleCount + 1);
    binOut.DoAbc.parsedABC.constantPool.doubles = getDoubles((uint32_t)binOut.DoAbc.parsedABC.constantPool.doubleCount, rawTag);

    binOut.DoAbc.parsedABC.constantPool.stringCount = getVariableUint30(rawTag);
    std::cout << "DoABC: ABC File: Constant Pool: String Count: " << static_cast<int>(binOut.DoAbc.parsedABC.constantPool.stringCount) << "\n";

    binOut.DoAbc.parsedABC.constantPool.strings.resize((uint32_t)binOut.DoAbc.parsedABC.constantPool.stringCount + 1);
    binOut.DoAbc.parsedABC.constantPool.strings = getStrings((uint32_t)binOut.DoAbc.parsedABC.constantPool.stringCount, rawTag);

    binOut.DoAbc.parsedABC.constantPool.namespaceCount = getVariableUint30(rawTag);
    std::cout << "DoABC: ABC File: Constant Pool: Namespace Count: " << static_cast<int>(binOut.DoAbc.parsedABC.constantPool.namespaceCount) << "\n";

    binOut.DoAbc.parsedABC.constantPool.namespaces.resize((uint32_t)binOut.DoAbc.parsedABC.constantPool.namespaceCount + 1);
    binOut.DoAbc.parsedABC.constantPool.namespaces = getNamespaces((uint32_t)binOut.DoAbc.parsedABC.constantPool.namespaceCount, rawTag);

    return binOut;
}
