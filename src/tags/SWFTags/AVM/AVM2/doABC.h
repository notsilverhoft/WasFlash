#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>

struct rawSWFTag;
struct SWFTag;

typedef struct {
    // int24_t:
    int32_t data:24;
} int24_t;
    
//uint30_t:
struct _uint30_t {
    uint32_t data;
    _uint30_t(uint32_t v) : data(v & 0x3FFFFFFF) {};
    _uint30_t() : data(0) {};
    explicit operator uint32_t() const { return data; };
    explicit operator int32_t() const { return static_cast<int32_t>(data); }
};


typedef _uint30_t uint30_t;

struct stringInfo {
    uint30_t size;
    std::vector<uint8_t> utf8;
};

struct namespaceInfo {
    uint8_t kind;
    uint30_t name;
};

struct namespaceSetInfo {
    uint30_t count;
    std::vector<uint30_t> ns;
};

struct multinameInfo {
    uint8_t type;
    std::vector<uint8_t> data;
};

struct cpoolInfo {
    uint30_t intCount;
    std::vector<int32_t> integers;
    uint30_t uintCount;
    std::vector<uint32_t> uintegers;
    uint30_t doubleCount;
    std::vector<double> doubles;
    uint30_t stringCount;
    std::vector<stringInfo> strings;
    uint30_t namespaceCount;
    std::vector<namespaceInfo> namespaces;
    uint30_t namespaceSetCount;
    std::vector<namespaceSetInfo> namespaceSets;
    uint30_t multinameCount;
    std::vector<multinameInfo> multinames;
};

struct optionDetail {
    uint30_t val;
    uint64_t type;
};

struct optionInfo {
    uint30_t optionCount;
    std::vector<optionDetail> options;
};

struct paramInfo {
    std::vector<uint30_t> paramName;
};

struct methodInfo {
    uint30_t paramCount;
    uint30_t returnType;
    std::vector<uint30_t> paramType;
    uint30_t name;
    uint8_t flags;
    optionInfo options;
    paramInfo paramNames;
};

struct itemInfo {
    uint30_t key;
    uint30_t value;
};

struct metadataInfo {
    uint30_t name;
    uint30_t itemCount;
    std::vector<itemInfo> items;
};

struct traitsInfo {
    uint30_t name;
    uint8_t type;
    std::vector<uint8_t> data;
    uint30_t metadataCount;
    std::vector<uint30_t> metadata;
};

struct instanceInfo {
    uint30_t name;
    uint30_t superName;
    uint8_t flags;
    uint30_t protectedNamespace;
    uint30_t interfaceCount;
    std::vector<uint30_t> interfaces;
    uint30_t iinit;
    uint30_t traitCount;
    std::vector<traitsInfo> traits;
};

struct classInfo {
    uint30_t cinit;
    uint30_t traitCount;
    std::vector<traitsInfo> traits;
};

struct scriptInfo {
    uint30_t init;
    uint30_t traitCount;
    std::vector<traitsInfo> traits;
};

struct exceptionInfo {
    uint30_t from;
    uint30_t to;
    uint30_t target;
    uint30_t excType;
    uint30_t varName;
};

struct methodBodyInfo {
    uint30_t method;
    uint30_t maxStack;
    uint30_t localCount;
    uint30_t initScopeDepth;
    uint30_t maxScopeDepth;
    uint30_t codeLength;
    std::vector<uint8_t> codes;
    uint30_t exceptionCount;
    std::vector<exceptionInfo> exceptions;
    uint30_t traitCount;
    std::vector<traitsInfo> traits;
};

struct abcFile {
    uint16_t minorVersion;
    uint16_t majorVersion;
    cpoolInfo constantPool;
    uint30_t methodCount;
    std::vector<methodInfo> methods;
    uint30_t metadataCount;
    std::vector<metadataInfo> metadata;
    uint30_t classCount;
    std::vector<instanceInfo> instances;
    std::vector<classInfo> classes;
    uint30_t scriptCount;
    std::vector<scriptInfo> scripts;
    uint30_t methodBodyCount;
    std::vector<methodBodyInfo> methodBodys;
};

SWFTag getDoABCTag(rawSWFTag& rawTag);