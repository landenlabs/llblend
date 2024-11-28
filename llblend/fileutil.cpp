//-------------------------------------------------------------------------------------------------
//  File: FileUtil.cpp
//  Desc: General utility functions
//
//  Created by Dennis Lang on 12/21/21.
//  Copyright © 2021 Dennis Lang. All rights reserved.
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


#include "FileUtil.hpp"
#include "commands.hpp"
#include "directory.hpp"
#include "ll_stdhdr.hpp"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <vector>



#ifdef HAVE_WIN
    const char SLASH_CHAR('\\');
    #include <assert.h>
    #define strncasecmp _strnicmp
    #if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
        #define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
    #endif
#else
    const char SLASH_CHAR('/');
    #include <time.h>
#endif

// ---------------------------------------------------------------------------
volatile bool Command::abortFlag = false;

#if defined(_WIN32) || defined(_WIN64)

// ---------------------------------------------------------------------------
std::string GetErrorMsg(DWORD error) {
    std::string errMsg;
    if (error != 0) {
        LPTSTR pszMessage;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&pszMessage,
            0, NULL);

        errMsg = pszMessage;
        LocalFree(pszMessage);
        int eolPos = (int)errMsg.find_first_of('\r');
        errMsg.resize(eolPos);
        errMsg.append(" ");
    }
    return errMsg;
}

// ---------------------------------------------------------------------------
size_t FileUtil::isWriteableFile(const struct stat& info) {
    size_t mask = _S_IFREG + _S_IWRITE;
    return ((info.st_mode & mask) == mask);
}

#else

// ---------------------------------------------------------------------------
size_t FileUtil::isWriteableFile(const struct stat& info) {
    size_t mask = S_IFREG + S_IWRITE;
    return ((info.st_mode & mask) == mask);
}
#endif

// ---------------------------------------------------------------------------
// Extract name part from path, name includes extension
lstring& FileUtil::getName(lstring& outName, const lstring& inPath) {
    size_t nameStart = inPath.rfind(SLASH_CHAR) + 1;
    if (nameStart == 0)
        outName = inPath;
    else
        outName = inPath.substr(nameStart);
    return outName;
}

// ---------------------------------------------------------------------------
// Return true if inName matches pattern in patternList
bool FileUtil::FileMatches(const lstring& inName, const PatternList& patternList, bool emptyResult) {
    if (patternList.empty() || inName.empty())
        return emptyResult;

    for (size_t idx = 0; idx != patternList.size(); idx++)
        if (std::regex_match(inName.begin(), inName.end(), patternList[idx]))
            return true;

    return false;
}

// ---------------------------------------------------------------------------
bool FileUtil::deleteFile(const char* path) {
#if defined(_WIN32) || defined(_WIN64)
    SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);
    if (0 == DeleteFile(path)) {
        DWORD err = GetLastError();
        if (err != ERROR_FILE_NOT_FOUND) {  // 2 = ERROR_FILE_NOT_FOUND
            std::cerr << err << " error trying to delete " << path << std::endl;
            return false;
        }
    }
#else
    unlink(path);
#endif
    return true;
}
