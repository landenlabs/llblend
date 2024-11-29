//-------------------------------------------------------------------------------------------------
//  File: Fcolor.hpp
//  Desc: FreeImage C++ wrapper on image pixel (color).
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

// #include "FImage.hpp"
#include "freeimage/FreeImage.h"

class FColor : public RGBQUAD {
public:

    FColor() {
        rgbRed = rgbGreen = rgbBlue = rgbReserved = 0;
    }

    FColor(BYTE red, BYTE green, BYTE blue, BYTE alpha = 0xff) {
        rgbRed = red;
        rgbGreen = green;
        rgbBlue = blue;
        rgbReserved = alpha;
    }

    FColor(const RGBQUAD& rgb, BYTE alpha) : RGBQUAD(rgb) {
        rgbReserved = alpha;
    }

    FColor(const FColor& other) {
        rgbRed = other.rgbRed;
        rgbGreen = other.rgbGreen;
        rgbBlue = other.rgbBlue;
        rgbReserved = other.rgbReserved;
    }

    FColor& operator=(const FColor& other) {
        *((DWORD*) this) = *((DWORD*) &other);
        return *this;
    }

    bool operator==(const FColor& other) const {
        return *((DWORD*) this) == *((DWORD*) &other);
    }
    bool operator!=(const FColor& other) const {
        return *((DWORD*) this) != *((DWORD*) &other);
    }

    RGBQUAD* quad() const {
        return (RGBQUAD*)this;  // Cast away const
    }

    void blendOver(RGBQUAD& botColor) const;

    static
    BYTE clamp(unsigned cBig) {
        return (cBig > 0xff) ? 0xff : (BYTE)cBig;
    }

    static // rate 0..100
    BYTE darken(unsigned rate, BYTE cByte) {
        unsigned cBig (cByte * rate / 100);
        return clamp(cBig);
    }

    static // rate 0..100
    FColor gray(unsigned rate, BYTE red, BYTE green, BYTE blue, BYTE alpha = 0xff) {
        return FColor(
                darken(rate, red),
                darken(rate, green),
                darken(rate, blue),
                alpha);
    }

    static
    FColor makeColor(BYTE red, BYTE green, BYTE blue, BYTE alpha = 0xff) {
        return FColor(red, green, blue, alpha);
    }

    size_t distanceRGB(const FColor& other) const {
        size_t dRed = rgbRed - other.rgbRed;
        size_t dGreen = rgbGreen - other.rgbGreen;
        size_t dBlue = rgbBlue - other.rgbBlue;
        return dRed * dRed + dGreen * dGreen + dBlue * dBlue;
    }
};
