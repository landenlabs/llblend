//-------------------------------------------------------------------------------------------------
//  File: FPalette.cpp
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

#include "fpalette.hpp"

static FPalette NOWRAD_PALETTE;
static FPalette NOWRAD_GRAY_PALETTE;
static Mapping NOWRAD_TO_GRAY_MAPPING;

static FPalette TEMPERATURE_PALETTE;
static FPalette TEMPERATURE_GRAY_PALETTE;

const FColor FPalette::TRANSPARENT(0, 0, 0, 0);
const FColor FPalette::BLACK(0, 0, 0, 255);
const FColor FPalette::GRAY(128, 128, 128, 255);
const FColor FPalette::WHITE(255, 255, 255, 255);


// https://ssds-catalogui-useast1.qa.ssds.weather.com/v2/catalogui/tilepaletter/palettes/rainDBz_nowrad
const FPalette& FPalette::getNowradPalette() {
    if (NOWRAD_PALETTE.empty()) {
        NOWRAD_PALETTE.push_back(TRANSPARENT);            //  0  black
        NOWRAD_PALETTE.push_back(FColor(99, 235, 99));    //  1  green
        NOWRAD_PALETTE.push_back(FColor(60, 198, 60));    //  2  green
        NOWRAD_PALETTE.push_back(FColor(28, 157, 52));    //  3  green
        NOWRAD_PALETTE.push_back(FColor(14, 104, 26));    //  4  FColor
        NOWRAD_PALETTE.push_back(FColor(0, 63, 0));       //  5  green

        NOWRAD_PALETTE.push_back(FColor(251, 235, 2));    //  6  yellow
        NOWRAD_PALETTE.push_back(FColor(238, 109, 2));    //  7  orange

        NOWRAD_PALETTE.push_back(FColor(210, 11, 6));     //  8  red
        NOWRAD_PALETTE.push_back(FColor(189, 8, 4));      //  9  red
        NOWRAD_PALETTE.push_back(FColor(169, 5, 3));      // 10  red
        NOWRAD_PALETTE.push_back(FColor(148, 2, 1));      // 11  red
        NOWRAD_PALETTE.push_back(FColor(128, 0, 0));      // 12  red

        NOWRAD_PALETTE.push_back(BLACK);                  // 13 black
        NOWRAD_PALETTE.push_back(GRAY);                   // 14 gray
        NOWRAD_PALETTE.push_back(WHITE);                  // 15 white

        // Freeze
        NOWRAD_PALETTE.push_back(FColor(188, 165, 240));  // purple
        NOWRAD_PALETTE.push_back(FColor(161, 137, 214));  // purple
        NOWRAD_PALETTE.push_back(FColor(130, 104, 186));  // purple
        NOWRAD_PALETTE.push_back(FColor(98, 70, 155));    // purple
        NOWRAD_PALETTE.push_back(FColor(82, 53, 140));    // purple

        // Mixed
        NOWRAD_PALETTE.push_back(FColor(255, 160, 207));  // red
        NOWRAD_PALETTE.push_back(FColor(224, 120, 172));  // red
        NOWRAD_PALETTE.push_back(FColor(192, 77, 134));   // red
        NOWRAD_PALETTE.push_back(FColor(155, 25, 90));    // red
        NOWRAD_PALETTE.push_back(FColor(146, 13, 79));    // red

        // Snow
        NOWRAD_PALETTE.push_back(FColor(138, 248, 255));  // blue
        NOWRAD_PALETTE.push_back(FColor(96, 181, 191));   // blue
        NOWRAD_PALETTE.push_back(FColor(40, 93, 106));    // blue
        NOWRAD_PALETTE.push_back(FColor(13, 49, 64));     // blue
        NOWRAD_PALETTE.push_back(FColor(13, 49, 64));     // blue
    }
    return NOWRAD_PALETTE;
}

const FPalette& FPalette::getNowradGrayPalette() {
    if (NOWRAD_GRAY_PALETTE.empty()) {
        BYTE a = 128 + 64;
        unsigned rate = 90; // 0..100
        NOWRAD_GRAY_PALETTE.push_back(TRANSPARENT);                             //  0  black
        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 251, 235, 2, a));      //  6  yellow
        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 238, 109, 2, a));      //  7  orange

        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 210, 11, 6, a));      //  8  red
        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 189, 8, 4, a));       //  9  red
        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 169, 5, 3, a));       // 10  red
        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 148, 2, 1, a));       // 11  red
        NOWRAD_GRAY_PALETTE.push_back(FColor::gray(rate, 128, 0, 0, a));       // 12  red
    }
    return NOWRAD_GRAY_PALETTE;
}

const Mapping& FPalette::getNowradToGrayMapping() {
    if (! NOWRAD_TO_GRAY_MAPPING.isReady) {
        NOWRAD_TO_GRAY_MAPPING.reset();
        NOWRAD_TO_GRAY_MAPPING.to[6] = 1;
        NOWRAD_TO_GRAY_MAPPING.to[7] = 2;
        NOWRAD_TO_GRAY_MAPPING.to[8] = 3;
        NOWRAD_TO_GRAY_MAPPING.to[9] = 4;
        NOWRAD_TO_GRAY_MAPPING.to[10] = 5;
        NOWRAD_TO_GRAY_MAPPING.to[11] = 6;
        NOWRAD_TO_GRAY_MAPPING.to[12] = 7;

        NOWRAD_TO_GRAY_MAPPING.to[15] = 0;  // white to transparent
    }
    return NOWRAD_TO_GRAY_MAPPING;
}



const FPalette& FPalette::getTemperaturePalette() {
    // http://dashboard-useast1.qa.ssds.weather.com/ssds/dashboard/proxy/meta/v2/ssdscatalog/api/v3/palettes/palettes/temp.xml
    if (TEMPERATURE_PALETTE.empty()) {
        TEMPERATURE_PALETTE.push_back(FColor(40, 10, 70));       // -70
        TEMPERATURE_PALETTE.push_back(FColor(40, 10, 100));
        TEMPERATURE_PALETTE.push_back(FColor(80, 50, 130));
        TEMPERATURE_PALETTE.push_back(FColor(120, 90, 160));
        TEMPERATURE_PALETTE.push_back(FColor(160, 130, 190));
        TEMPERATURE_PALETTE.push_back(FColor(200, 170, 220));
        TEMPERATURE_PALETTE.push_back(FColor(110, 0, 70));      // -10
        TEMPERATURE_PALETTE.push_back(FColor(160, 50, 140));    // 0
        TEMPERATURE_PALETTE.push_back(FColor(205, 95, 200));
        TEMPERATURE_PALETTE.push_back(FColor(170, 225, 250));
        TEMPERATURE_PALETTE.push_back(FColor(100, 125, 190));
        TEMPERATURE_PALETTE.push_back(FColor(20, 20, 150));
        TEMPERATURE_PALETTE.push_back(FColor(115, 105, 100));
        TEMPERATURE_PALETTE.push_back(FColor(215, 215, 50));
        TEMPERATURE_PALETTE.push_back(FColor(220, 150, 0));
        TEMPERATURE_PALETTE.push_back(FColor(220, 40, 0));
        TEMPERATURE_PALETTE.push_back(FColor(150, 0, 0));       // 90
        TEMPERATURE_PALETTE.push_back(FColor(245, 125, 200));
        TEMPERATURE_PALETTE.push_back(FColor(210, 210, 210));
        TEMPERATURE_PALETTE.push_back(FColor(240, 240, 175));
        TEMPERATURE_PALETTE.push_back(FColor(240, 240, 175));    // 130
    }
    return TEMPERATURE_PALETTE;
}
const FPalette& FPalette::getTemperatureGrayPalette() {
    if (TEMPERATURE_GRAY_PALETTE.empty()) {
        BYTE a = 128 + 64;
        unsigned rate = 90; // 0..100
        TEMPERATURE_GRAY_PALETTE.push_back(TRANSPARENT);                             //  0  black
        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 251, 235, 2, a));
        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 238, 109, 2, a));

        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 210, 11, 6, a));
        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 189, 8, 4, a));
        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 169, 5, 3, a));
        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 148, 2, 1, a));
        TEMPERATURE_GRAY_PALETTE.push_back(FColor::gray(rate, 128, 0, 0, a));
    }
    return TEMPERATURE_GRAY_PALETTE;
}
#include <limits>

unsigned FPalette::findClosest(const FColor& color4, size_t maxDst,  unsigned failIdx) const {
    size_t minDist =  std::numeric_limits<size_t>::max();
    unsigned minIdx = failIdx;
    for (unsigned idx = 0; idx < size(); idx++) {
        const FColor& color = at(idx);
        size_t dist = color4.distanceRGB(color);

        if (dist < minDist && dist < maxDst && color4.rgbReserved == color.rgbReserved) {
            minDist = dist;
            minIdx = idx;
        }
    }


    return minIdx;
}

unsigned FPalette::findAlpha(const FColor& color4, unsigned failIdx) const {
    if (color4.rgbReserved != 0xff && hasTransparency) {
        for (unsigned idx = 0; idx < size(); idx++) {
            const FColor& color = at(idx);
            if (color4.rgbReserved == color.rgbReserved) {
                return idx;
            }
        }
    }
    return failIdx;
}
