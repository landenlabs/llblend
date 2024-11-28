
//-------------------------------------------------------------------------------------------------
//  FImage.hpp - FreeImage C++ wrapper
//
//  Created by Dennis Lang on 12/21/21.
//  Copyright © 2021 Dennis Lang. All rights reserved.
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

#include "FImage.hpp"
#include <iostream>

unsigned FImage::DBG_CNT = 0;

// ----------------------------------------------------------
FImage::FImage(FIBITMAP* _imgPtr) : imgPtr(_imgPtr) {
    DBG_CNT++;
}

// ----------------------------------------------------------
void FImage::Close() {
    if (Valid()) {
        FreeImage_Unload(imgPtr);
        imgPtr = nullptr;
        // std::cout << "close cnt=" << --DBG_CNT << std::endl;
    }
}

// ----------------------------------------------------------
bool FImage::LoadFromHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle, int flags) {
    Close();
    DBG_CNT++;
    imgPtr = FreeImage_LoadFromHandle(fif, io, handle, flags);
    return Valid();
}

// ------------------------------------------------------
void FImage::FillImage(const FColor& color) {
    unsigned width = GetWidth();
    unsigned height = GetHeight();
    unsigned bitsPerPixel = GetBitsPerPixel();

    width *= bitsPerPixel / 8;

    switch (bitsPerPixel) {
    case 8:
    case 32:
        // TODO - use fill color.
        for (unsigned y = 0; y < height; y++) {
            BYTE* bits = ScanLine(y);
            memset(bits, 0, width);
        }
        break;
    default:
        // TODO - handle all image types
        std::cerr << "Unsupported FILL pixel size " << bitsPerPixel << std::endl;
    }
}

// ----------------------------------------------------------
FPalette& FImage::getPalette(FPalette& palette) const {
    unsigned colors = GetColorsUsed();
    const RGBQUAD* palettePtr = GetPalette();

    palette.clear();
    palette.reserve(colors);

    palette.hasTransparency = IsTransparent();
    if (palette.hasTransparency) {
        const BYTE* transparentPtr = GetTransparencyTable();
        for (unsigned clrIdx = 0; clrIdx < colors; clrIdx++) {
            palette.push_back(FColor(palettePtr[clrIdx], transparentPtr[clrIdx]));
        }
    } else {
        for (unsigned clrIdx = 0; clrIdx < colors; clrIdx++) {
            palette.push_back(FColor(palettePtr[clrIdx], 0xff));
        }
    }
    return palette;
}

// ----------------------------------------------------------
unsigned FImage::setPalette(const FPalette& palette) {
    unsigned colors = GetColorsUsed();
    RGBQUAD* palettePtr = Palette();
    if (palettePtr != NULL && colors > 0 && palette.size() > 0) {
        BYTE transparency[256];
        memset(transparency, 0xff, sizeof(transparency));
        unsigned i = 0;
        for (; i < colors && i < palette.size(); i++) {
            palettePtr[i] = palette[i];
            transparency[i] = palette[i].rgbReserved;
        }

        transparency[0] = 0x00;
        SetTransparencyTable(transparency, i);
        return i;
    }
    return 0;
}

// ------------------------------------------------------
void FImage::AdjustAlphaP32(float percent) {
    unsigned scale = (unsigned)(256 * percent);
    unsigned width  = GetWidth();
    unsigned height = GetHeight();
    for (unsigned y = 0; y < height; y++) {
        RGBQUAD* argbPtr = (RGBQUAD*)ScanLine(y);
        for (unsigned x = 0; x < width; x++) {
            RGBQUAD& argb = *argbPtr++;
            argb.rgbReserved = argb.rgbReserved * scale / 256;
        }
    }
}

// ----------------------------------------------------------
void FImage::DrawRectangleI8(
    const FBrush& brush,
    unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
    BYTE pixel = brush.fillIndex;


    for (unsigned y = y1; y < y2; y++) {
        BYTE* linePtr = ScanLine(y);
        linePtr += x1;
        unsigned width = x2 - x1 + 1;
        while (width-- > 0)
            *linePtr++ = brush.fillIndex;
    }

    if (brush.lineWidth > 0) {
        pixel = brush.lineIndex;
        for (unsigned x = x1; x < x2; x++) {
            for (unsigned w = 0; w < brush.lineWidth; w++) {
                SetPixelIndex(x, y1 + w, pixel);
                SetPixelIndex(x, y2 - w, pixel);
            }
        }
        for (unsigned y = y1; y < y2; y++) {
            for (unsigned w = 0; w < brush.lineWidth; w++) {
                SetPixelIndex(x1 + w, y, pixel);
                SetPixelIndex(x2 - w, y, pixel);
            }
        }
    }
}

// ----------------------------------------------------------
void FImage::DrawRectangleP32(
    const FBrush& brush,
    unsigned x1, unsigned y1, unsigned x2, unsigned y2) {

    for (unsigned y = y1; y < y2; y++) {
        FColor* linePtr = (FColor*)ScanLine(y);
        linePtr += x1;

        unsigned width = x2 - x1 + 1;
        while (width-- > 0)
            *linePtr++ = brush.fillColor;
    }

    if (brush.lineWidth > 0) {
        for (unsigned x = x1; x < x2; x++) {
            for (unsigned w = 0; w < brush.lineWidth; w++) {
                SetPixelColor(x, y1 + w, brush.lineColor);
                SetPixelColor(x, y2 - w, brush.lineColor);
            }
        }
        for (unsigned y = y1; y < y2; y++) {
            for (unsigned w = 0; w < brush.lineWidth; w++) {
                SetPixelColor(x1 + w, y, brush.lineColor);
                SetPixelColor(x2 - w, y, brush.lineColor);
            }
        }
    }
}