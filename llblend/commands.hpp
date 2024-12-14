//-------------------------------------------------------------------------------------------------
// File: Directory.h
// Desc: This class is used to obtain the names of files in a directory.
//
// Usage::
//      Create a Directory_files object by providing the name of the directory
//      to use.  'next_file_name()' returns the next file name found in the
//      directory, if any.  You MUST check for the existance of more files
//      by using 'more_files()' between each call to "next_file_name()",
//      it tells you if there are more files AND sequences you to the next
//      file in the directory.
//
//      The normal usage will be something like this:
//          Directory_files dirfiles( dirName);
//          while (dirfiles.more_files())
//          {   ...
//              lstring filename = dirfiles.name();
//              ...
//          }
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2021
// https://landenlabs.com
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

#pragma once

#include "ll_stdhdr.hpp"
#include "directory.hpp"
#include "blendfutil.hpp"
#include "blendcfg.hpp"

#include <vector>
#include <regex>
#include <fstream>  

// Helper types
typedef std::vector<lstring> StringList;
typedef std::vector<std::regex> PatternList;
typedef unsigned int uint;
typedef std::vector<unsigned> IntList;
typedef char Byte;
const unsigned BLOCK_SIZE = 1024;


// ---------------------------------------------------------------------------
class Command {
public:
    // Runtime options
    PatternList includeFilePatList;
    PatternList excludeFilePatList;
    lstring DECRYPT_KEY;

    bool showFile = false;
    bool verbose = false;

    lstring separator = "\n";
    lstring preDivider = "";
    lstring postDivider = "\n--\n";

    static volatile bool abortFlag;

private:
    lstring none;
    char code;

public:


    Command(char c) : code(c) {
    }

    virtual  bool begin(StringList& fileDirList)  {
        return fileDirList.size() > 0;
    }

    virtual size_t add( const lstring& file, DIR_TYPES dtypes) = 0;

    virtual bool end() {
        return true;
    }

    Command& share(const Command& other) {
        includeFilePatList = other.includeFilePatList;
        excludeFilePatList = other.excludeFilePatList;
        DECRYPT_KEY = other.DECRYPT_KEY;
        showFile = other.showFile;
        verbose = other.verbose;

        separator = other.separator;
        preDivider = other.preDivider;
        postDivider = other.postDivider;
        return *this;
    }
};

// Forward declaration
/*
namespace Magick {
class Image;
}
*/

// ---------------------------------------------------------------------------
class CmdDumpF : public Command {
    const BlendCfg& blendCfg;

public:
    CmdDumpF(const BlendCfg& cfg) : Command('d'), blendCfg(cfg) {}
    size_t add(const lstring& file, DIR_TYPES dtype);
};


// ---------------------------------------------------------------------------
class CmdBlendF : public Command {
    const BlendCfg& blendCfg;
    FImageRef overlayImgRef;
    StringList paths;

public:
    CmdBlendF(const BlendCfg& cfg) : Command('b'), blendCfg(cfg) {}
    bool begin(StringList& fileDirList);
    size_t add(const lstring& file, DIR_TYPES dtype);
    bool end();
};

