//-------------------------------------------------------------------------------------------------
//
// File: md5.cpp   Author: Dennis Lang  Desc: Compute md5 hash of file contents
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

#ifdef HAVE_WIN
    // 4291 - No matching operator delete found
    #pragma warning( disable : 4291 )
#endif
#define _CRT_SECURE_NO_WARNINGS

#include "md5.hpp"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <vector>

// #include "ll_stdhdr.hpp"
#include "hash.hpp"

typedef unsigned int uint;
typedef unsigned int DWORD;
typedef unsigned char Byte;

//-------------------------------------------------------------------------------------------------
// Uses static buffers, not thread safe.

const char* Md5::compute(const char* filePath) {
    std::ifstream in(filePath, ios::binary | ios::in);

    const uint sBufSize = 4096 * 16;
    static std::vector<Byte> vBuffer(sBufSize);
    char* buffer = (char*)vBuffer.data();

    md5_state_t state;
    md5_init(&state);

    DWORD totSize = 0;
    DWORD rlen = sBufSize;
    while (rlen == sBufSize && ! in.eof() && ! in.fail() ) {
        in.read(buffer, sBufSize);  //  ReadFile(fHnd, buffer, sBufSize, &rlen, 0) != 0)
        rlen = (DWORD)in.gcount();
        md5_append(&state, (const md5_byte_t*)buffer, rlen);
        totSize += rlen;
    }

    md5_byte_t digest[16];
    md5_finish(&state, digest);

    static char hex_output[16 * 2 + 1 + 15];
    for (uint idx = 0; idx < 16; ++idx)
        sprintf(hex_output + idx * 2, "%02x", digest[idx]);

    // sprintf(hex_output + 32, ", %8u,", totSize);
    return hex_output;
}