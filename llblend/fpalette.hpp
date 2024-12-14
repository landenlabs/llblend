//-------------------------------------------------------------------------------------------------
//  File: FPalette.hpp
//  Desc: FreeImage C++ wrapper on 8bit image palettes.
//
//  Wrapper created by Dennis Lang on 12/21/21.
//  Copyright © 2021 Dennis Lang. All rights reserved.
//
//  FreeImage 3  Design and implementation by
//  - Floris van den Berg (flvdberg@wxs.nl)
//  - Herv<E9> Drolon (drolon@infonie.fr)
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

#pragma once

#include "fcolor.hpp"
#include <vector>

class Mapping {
    static const unsigned LEN = 256;
public:
    BYTE from[LEN];
    BYTE to[LEN];
    bool isReady = false;

    void reset() {
        isReady = true;
        for (unsigned idx = 0; idx < sizeof(from); idx++) from[idx] = (BYTE)idx;
        memset(to, 0, sizeof(to));
    }
};

class FPalette : public std::vector<FColor> {
public:
    static const FColor TRANSPARENT;    // (0,0,0,0);
    static const FColor BLACK;          // (0,0,0,255);
    static const FColor GRAY;           // (128,128,128,255);
    static const FColor WHITE;          // (255,255,255,255);
    static const FPalette rainPalette;
    static const FPalette EMPTY;

    static const FPalette& getNowradPalette();
    static const FPalette& getNowradGrayPalette();
    static const Mapping& getNowradToGrayMapping();

    static const FPalette& getTemperaturePalette();
    static const FPalette& getTemperatureGrayPalette();

    bool hasTransparency;

    FPalette() : hasTransparency(false) {}

    FPalette(const FColor* colorPtr, unsigned nColors, bool _hasTransparency = false)
        : hasTransparency(_hasTransparency) {
        reserve(nColors);
        for (unsigned idx = 0; idx < nColors; idx++) {
            push_back(colorPtr[idx]);
        }
    }

    unsigned findClosest(const FColor& color4, size_t maxDst = 256 * 256L, unsigned failIdx = 256) const;
    unsigned findAlpha(const FColor& color4, unsigned failIdx = 256) const;

    unsigned findColor(const FColor& color4, unsigned defIdx = 0) const {

        for (unsigned idx = 0; idx < size(); idx++) {
            const FColor& ours = at(idx);
            if (color4 == ours)
                return idx;
        }
        return defIdx;
    }

    RGBQUAD* quads() const {
        return (RGBQUAD*)data();  // Cast away const
    }
};
