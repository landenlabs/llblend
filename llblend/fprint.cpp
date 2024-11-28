//-------------------------------------------------------------------------------------------------
//  File: FPrint.cpp
//  Desc: Print (display) FreeImage information.
//
//  FPrint created by Dennis Lang on 12/21/21.
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

#include "FPrint.hpp"

#include <iostream>

// -------------------------------------------------------------------------------------------------
const char* FPrint::toString(const RGBQUAD& color, const char* fmt) {
    static char str[40];
    snprintf(str, sizeof(str), fmt, color.rgbRed, color.rgbGreen, color.rgbBlue, color.rgbReserved);
    return str;
}

// -------------------------------------------------------------------------------------------------
const char* FPrint::toString(FREE_IMAGE_COLOR_TYPE colorType) {
    switch (colorType) {
    case FIC_MINISBLACK:
        return "MinIsBlack";
    case FIC_MINISWHITE:
        return "MinIsWhite";
    case FIC_PALETTE:
        return "Palette";
    case FIC_RGB:
        return "RGB";
    case FIC_RGBALPHA:
        return "RGBA";
    case FIC_CMYK:
        return "CMYK";
    }
    return "Unknown Color Type";
}

// -------------------------------------------------------------------------------------------------
const char* FPrint::toString(FREE_IMAGE_TYPE imgType) {
    switch (imgType) {
    case FIT_UNKNOWN:
        return "unknown";
    case FIT_BITMAP:
        return "bitmap";
    case FIT_UINT16:
        return "uint16";
    case FIT_INT16:
        return "int16";
    case FIT_UINT32:
        return "uint32";
    case FIT_INT32:
        return "int32";
    case FIT_FLOAT:
        return "float";
    case FIT_DOUBLE:
        return "double";
    case FIT_COMPLEX:
        return "complex";
    case FIT_RGB16:
        return "RGB16";
    case FIT_RGBA16:
        return "RGBA16";
    case FIT_RGBF:
        return "RGBF";
    case FIT_RGBAF:
        return "RGBAF";
    }
    return "Unknown Image Type";
}

// -------------------------------------------------------------------------------------------------
unsigned FPrint::printHisto(DWORD* histo, unsigned length, const RGBQUAD* palettePtr) {
    unsigned activeColors = 0;
    size_t totalCnt = 0;
    if (histo != NULL) {
        std::cout << "Histogram (" << length << ")\n";
        setlocale(LC_NUMERIC, "");

        for (unsigned i = 0; i < length; i++)
            totalCnt += histo[i];

        for (unsigned i = 0; i < length; i++) {
            DWORD cnt = histo[i];
            if (cnt != 0) {
                activeColors++;
                printf("  %3u: ", i);
                if (palettePtr != NULL)
                    printf("%s", toString(palettePtr[i]));
                // printf(" RGB(%3d, %3d, %3d)",  palettePtr[i].rgbRed, palettePtr[i].rgbGreen, palettePtr[i].rgbBlue);

                printf(" %'7u #  %3.1f %%\n", cnt, cnt * 100.0f / totalCnt);
                // totalCnt += cnt;
            }
        }
        printf(" Total  RGB(red, grn, blu) %'7lu #\n", totalCnt);
    }
    return activeColors;
}

// -------------------------------------------------------------------------------------------------
unsigned FPrint::printHisto(const FImage& img, unsigned colors) {
    DWORD histo[256];
    memset(histo, 0, sizeof(histo));
    img.GetHistogram(histo, FICC_BLACK);
    FPalette palette;
    img.getPalette(palette);
    return printHisto(histo, colors, palette.quads());
}

// -------------------------------------------------------------------------------------------------
void FPrint::printInfo(const FImage& img, const char* name) {
    FREE_IMAGE_TYPE imgType = img.GetImageType();
    unsigned bitsPerPixel = img.GetBitsPerPixel();
    unsigned pixelWidth = img.GetWidth();
    unsigned pixelHeight = img.GetHeight();
    unsigned colors = img.GetColorsUsed();
    unsigned bytesPerLine = img.GetBytesPerLine();
    FREE_IMAGE_COLOR_TYPE colorType = img.GetColorType();
    bool hastransparency = img.IsTransparent();

    // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
    unsigned bytesPerPixel = (pixelWidth != 0) ? bytesPerLine / pixelWidth : 0;

    std::cout
            << "\nName: " << name
                << "\n  ImageType: " << toString(imgType)
                << "\n  BitsPerPixel: " << bitsPerPixel
                << "\n  BytesPerPixel: " << bytesPerPixel
                << "\n  Width: " << pixelWidth
                << "\n  Height: " << pixelHeight
                << "\n  bytesPerLine: " << bytesPerLine
                << "\n  Colors: " << colors
                << "\n  ColorType: " << toString(colorType)
                << "\n  " << (hastransparency ? "Has Transparency" : "No Transparency");

    FColor bgColor;
    bool hasBgColor = img.GetBackgroundColor(bgColor);
    bool hasPixels = img.HasPixels();
    if (hasBgColor)
        std::cout << "\n  BackgroundColor: " << toString(bgColor);
    if (! hasPixels)
        std::cout << "\n  Pixels: NONE (header only)";
    std::cout << std::endl;
}

// -------------------------------------------------------------------------------------------------
void FPrint::printPalette(const RGBQUAD* palettePtr, unsigned colors) {
    if (palettePtr != NULL) {
        std::cout << "Colors (" << colors << ")\n";
        for (int i = 0; i < colors; i++) {
            printf("  [%3d] %s\n", i, toString(palettePtr[i]));
        }
    }
}

// -------------------------------------------------------------------------------------------------
unsigned FPrint::printPalette(const FImage& img, unsigned _colors) {
    unsigned colors = img.GetColorsUsed();
    colors = (_colors == 0) ? colors : _colors;
    FPalette palette;
    img.getPalette(palette);
    printPalette(palette.quads(), colors);
    return colors;
}

