//-------------------------------------------------------------------------------------------------
//
// File: BlendCfg.cpp
// Author: Dennis Lang
// Desc: Parse json config file.
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// http://landenlabs.com
//
// This file is part of llblendF project.
//
// ----- License ----
//
// Copyright (c) 2021  Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// 4291 - No matching operator delete found
// #pragma warning(disable : 4291)
#define _CRT_SECURE_NO_WARNINGS

// Project files
#include "BlendCfg.hpp"

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>


#ifdef WIN32
    const char SLASH_CHAR('\\');
    #include <assert.h>
    #define strncasecmp _strnicmp
    #if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
        #define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
    #endif
#else
    const char SLASH_CHAR('/');
#endif

// -------------------------------------------------------------------------------------------------
// Parse json word surrounded by quotes.
static void getJsonWord(JsonBuffer& buffer, char delim, JsonToken& word) {
    const char* lastPtr = strchr(buffer.ptr(), delim);
    word.clear();
    int len = int(lastPtr - buffer.ptr());
    word.append(buffer.ptr(len + 1), len);
    word.isQuoted = true;
}

// Forward definition
static JsonToken parseJson(JsonBuffer& buffer, JsonFields& jsonFields);

// -------------------------------------------------------------------------------------------------
// Parse json array
static void getJsonArray(JsonBuffer& buffer, JsonArray& array) {
    JsonFields jsonFields;
    for (;;) {
        JsonToken token = parseJson(buffer, jsonFields);
        if (token.mToken == JsonToken::Value) {
            JsonValue* jsonValue = new JsonValue(token);
            array.push_back(jsonValue);
        } else {
            return;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// Parse json group
static void getJsonGroup(JsonBuffer& buffer, JsonFields& fields) {
    for (;;) {
        JsonToken token = parseJson(buffer, fields);
        if (token.mToken == JsonToken::EndGroup) {
            return;
        }
    }
}

// -------------------------------------------------------------------------------------------------
static void addJsonValue(JsonFields& jsonFields, JsonToken& fieldName, JsonToken& value) {
    if (! fieldName.empty() && ! value.empty()) {
        jsonFields[fieldName] = new JsonToken(value);
        fieldName.clear();
        value.clear();
    }
}

// -------------------------------------------------------------------------------------------------
static JsonToken parseJson(JsonBuffer& buffer, JsonFields& jsonFields) {
    JsonToken fieldName = "";
    JsonToken fieldValue;
    JsonToken tmpValue;

    while (buffer.pos < buffer.size()) {
        char chr = buffer.nextChr();
        switch (chr) {
        default:
            fieldValue += chr;
            break;

        case '/':
            if (buffer.peekChr() == '/') {
                buffer.moveTo('\n');
            } else {
                fieldValue += chr;
            }
            break;
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            addJsonValue(jsonFields, fieldName, fieldValue);
            break;
        case ',':
            tmpValue = fieldValue;
            addJsonValue(jsonFields, fieldName, fieldValue);
            return tmpValue;

        case ':':
            fieldName = fieldValue;
            fieldValue.clear();
            break;

        case '{': {
            JsonFields* pJsonFields = new JsonFields();
            jsonFields[fieldName] = pJsonFields;
            getJsonGroup(buffer, *pJsonFields);
        } break;
        case '}':
            addJsonValue(jsonFields, fieldName, fieldValue);
            return END_GROUP;

        case '"':
            getJsonWord(buffer, '"', fieldValue);
            break;
        case '[': {
            JsonArray* pJsonArray = new JsonArray();
            jsonFields[fieldName] = pJsonArray;
            getJsonArray(buffer, *pJsonArray);
        } break;
        case ']':
            return END_ARRAY;
        }
    }

    return END_PARSE;
}

// -------------------------------------------------------------------------------------------------
bool BlendCfg::parseConfig(const lstring& cfgFilename) {
    ifstream in;
    ofstream out;

    try {
        if (stat(cfgFilename, &filestat) == 0) {
            in.open(cfgFilename);
            if (in.good()) {

                buffer.resize(filestat.st_size + 1);
                streamsize inCnt = in.read(buffer.data(), buffer.size()).gcount();
                assert(inCnt < buffer.size());
                in.close();
                buffer.push_back('\0');

                parseJson(buffer, fields);
                in.close();
                return true;
            } else {
                cerr << "Config " << strerror(errno) << ", Unable to open " << cfgFilename << endl;
            }
        }
    } catch (exception ex) {
        cerr << "Config " << ex.what() << ", Error in file:" << cfgFilename << endl;
    }

    return false;
}

void BlendCfg::print() {
    std::cout << fields.toString() << std::endl;
}

// -------------------------------------------------------------------------------------------------
const Mapping&   BlendCfg::getMapping() const {
    if (! mapping.isReady) {

    }
    return mapping;
}

// -------------------------------------------------------------------------------------------------
const FPalette&   BlendCfg::getOverlayPalette() const {

    if (overlayPalette.empty()) {
        MapList mapList;
        StringList keys;
        fields.at("overlay-palette")->toMapList(mapList, keys);
        std::cout << "keys=" << keys.size() << std::endl;
    }

    return overlayPalette;
}
