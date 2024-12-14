//-------------------------------------------------------------------------------------------------
//
// File: commands.cpp   Author: Dennis Lang  Desc: Process file scan
//
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

//
// ImageMagick Magick++ API Documentation
//   https://www.imagemagick.org/Magick++/Documentation.html
//

#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <vector>

#include "commands.hpp"
#include "directory.hpp"
#include "ll_stdhdr.hpp"
#include "split.hpp"

// #include "blendfutil.hpp"
#include "fprint.hpp"
#include "fileutil.hpp"


//-------------------------------------------------------------------------------------------------
// Locate matching files which are not in exclude list.
// Locate pair of files one encrypt with AXX and the native file
size_t CmdDumpF::add(const lstring& fullname, DIR_TYPES dtype) {
    size_t fileCount = 0;
    lstring name;
    FileUtil::getName(name, fullname);

    if (dtype == IS_FILE && ! name.empty()
        && ! FileUtil::FileMatches(name, excludeFilePatList, false)
        && FileUtil::FileMatches(name, includeFilePatList, true)) {
        fileCount++;

        struct stat info;
        if (stat(fullname, &info) == 0 && FileUtil::isWriteableFile(info)) {
            if (showFile)
                std::cout << fullname.c_str() << std::endl;
        } else {
            if (showFile)
                std::cout << "ReadOnly " << fullname.c_str() << std::endl;
        }

        BlendFUtil::Dump(fullname);
        BlendFUtil::Palette(fullname);
    }

    return fileCount;
}


//-------------------------------------------------------------------------------------------------
bool CmdBlendF::begin(StringList& fileDirList) {

    if (false) {
        // TODO - populate reference color palette image.
        // imageRefPalette  = new Image();
        // imageRefPalette->type(PaletteType);
    }
    overlayImgRef = nullptr;
    return fileDirList.size() > 0;
}


//-------------------------------------------------------------------------------------------------
// Locate matching files which are not in exclude list.
// Locate pair of files one encrypt with AXX and the native file
size_t CmdBlendF::add(const lstring& fullname, DIR_TYPES dtype) {
    size_t fileCount = 0;
    lstring name;
    FileUtil::getName(name, fullname);

    if (dtype == IS_FILE && ! name.empty()
        && ! FileUtil::FileMatches(name, excludeFilePatList, false)
        && FileUtil::FileMatches(name, includeFilePatList, true)) {
        fileCount++;

        struct stat info;
        if (stat(fullname, &info) == 0 && FileUtil::isWriteableFile(info)) {
            if (showFile)
                std::cout << fullname.c_str() << std::endl;
        } else {
            if (showFile)
                std::cout << "ReadOnly " << fullname.c_str() << std::endl;
        }
        paths.push_back(fullname);

    }

    return fileCount;
}

//-------------------------------------------------------------------------------------------------
bool CmdBlendF::end() {
    bool okay = false;

    std::sort(paths.begin(), paths.end());

    /*
    for (const auto &item : paths) {
        cout << item << "; ";
    }
    */

    for (const std::string& fullname : paths) {
        // BlendFUtil::dump(fullname);
        BlendFUtil::Blend(fullname.c_str(), blendCfg, overlayImgRef);
    }

    if (overlayImgRef != nullptr) {
        FPrint::printInfo(overlayImgRef, "overlayImg");
        // FPrint::printPalette(*overlayImgRef);
        // FPrint::printHisto(*overlayImgRef);
        okay = BlendFUtil::saveTo(overlayImgRef, "/tmp/ftestOverlay.png");
        overlayImgRef->Close();
        *overlayImgRef = nullptr;
    }
    return okay;
}