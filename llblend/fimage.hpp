//-------------------------------------------------------------------------------------------------
//  File: FImage.hpp
//  Desc: FreeImage C++ wrapper
//
//  Wrapper created by Dennis Lang on 12/21/21.
//  Copyright © 2021 Dennis Lang. All rights reserved.
//
//  FreeImage 3  Design and implementation by
//  - Floris van den Berg (flvdberg@wxs.nl)
//  - Herv<E9> Drolon (drolon@infonie.fr)
//
//  Download FreeImage source code distribution
//    1. https://sourceforge.net/projects/freeimage/
//    2. edit Makfile.osx and change arch from X86_64 to arm64
//    2. build distribution,   make
//    3. install,  make install
//   Files get stored in-
//      /usr/local/lib/libfreeimage.a
//      /usr/local/include/FreeImage.h
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

#include "freeimage/FreeImage.h"

#include "FPalette.hpp"
#include "FColor.hpp"
#include "FBrush.hpp"

#include <iostream>


// Forward ref
class FImage;

// typedef std::unique_ptr<FImage> FImageRef;
class FImageRef : public std::unique_ptr<FImage> {
public:
    FImageRef() = default;
    // FImageRef& operator=(const FImageRef&) = default;   // default copy semantics
    // FImageRef(const FImageRef&) = default;

    FImageRef(FImage* imgPtr) : unique_ptr(imgPtr) { }
    FImage& ref() { return *get(); }
    const FImage& cref() const { return *get(); }
    operator FImage& () { return ref(); }
    operator const FImage& () const { return cref(); }
};


class FImage {
public:

    static unsigned DBG_CNT;
    FIBITMAP* imgPtr;

    FImage() : imgPtr(nullptr)
    { }
    FImage(FIBITMAP* _imgPtr);
    ~FImage() {
        Close();
    }

    void Close();

    bool Valid() const
    { return (imgPtr != nullptr); }

    FREE_IMAGE_TYPE GetImageType() const
    { return FreeImage_GetImageType(imgPtr); }
    bool GetBackgroundColor(FColor& bgColor) const
    { return FreeImage_GetBackgroundColor(imgPtr, &bgColor); }
    bool HasPixels() const
    { return FreeImage_HasPixels(imgPtr); }

    unsigned  GetWidth() const
    { return FreeImage_GetWidth(imgPtr); }
    unsigned  GetHeight() const
    { return FreeImage_GetHeight(imgPtr); }


    unsigned GetBitsPerPixel() const
    { return FreeImage_GetBPP(imgPtr); }
    unsigned GetBytesPerLine() const
    { return FreeImage_GetLine(imgPtr);}
    const BYTE* ReadScanLine(unsigned y) const
    { return FreeImage_GetScanLine(imgPtr, y); }
    BYTE* ScanLine(unsigned y)
    { return FreeImage_GetScanLine(imgPtr, y); }

    bool GetPixelIndex(unsigned x, unsigned y, BYTE& value) const
    { return FreeImage_GetPixelIndex(imgPtr, x, y, &value); }
    bool SetPixelIndex(unsigned x, unsigned y, BYTE value)
    { return FreeImage_SetPixelIndex(imgPtr, x, y, &value); }
    bool GetPixelColor(unsigned x, unsigned y, FColor& value) const
    { return FreeImage_GetPixelColor(imgPtr, x, y, (RGBQUAD*)&value); }
    bool SetPixelColor(unsigned x, unsigned y, const FColor& value)
    { return FreeImage_SetPixelColor(imgPtr, x, y, (RGBQUAD*)&value); }


    bool GetHistogram(DWORD* histo, FREE_IMAGE_COLOR_CHANNEL channel = FICC_BLACK) const
    {   return FreeImage_GetHistogram(imgPtr, histo, channel); }

    FREE_IMAGE_COLOR_TYPE GetColorType() const
    { return FreeImage_GetColorType(imgPtr); }
    unsigned GetColorsUsed() const
    { return FreeImage_GetColorsUsed(imgPtr); }
    const RGBQUAD* GetPalette() const
    { return FreeImage_GetPalette(imgPtr); }
    RGBQUAD* Palette()
    { return FreeImage_GetPalette(imgPtr); }
    bool SetBackgroundColor(const FColor& bkColor)
    { return FreeImage_SetBackgroundColor(imgPtr, (RGBQUAD*)&bkColor); }

    void SetTransparent(bool b)
    { FreeImage_SetTransparent(imgPtr, b); }
    void SetTransparentIndex(unsigned clrIdx)
    { FreeImage_SetTransparentIndex(imgPtr, clrIdx); }
    bool IsTransparent() const
    { return FreeImage_IsTransparent(imgPtr); }
    void SetTransparencyTable(const BYTE* transArray, unsigned arrayLen)
    { FreeImage_SetTransparencyTable(imgPtr, (BYTE*)transArray, arrayLen); }
    const BYTE* GetTransparencyTable() const
    { return FreeImage_GetTransparencyTable(imgPtr); }
    BYTE* TransparencyTable()
    { return FreeImage_GetTransparencyTable(imgPtr); }
    unsigned ApplyPaletteIndexMapping(const BYTE *srcindices, const BYTE *dstindices, unsigned count, bool swap = false)
    { return FreeImage_ApplyPaletteIndexMapping(imgPtr, (BYTE*)srcindices, (BYTE*)dstindices, count, swap); }

    FImage ConvertTo24Bits() const
    { return FImage(FreeImage_ConvertTo24Bits(imgPtr)); }
    FImage ConvertTo32Bits() const
    { return FImage(FreeImage_ConvertTo32Bits(imgPtr)); }

    FImage ColorQuantizeEx(FREE_IMAGE_QUANTIZE quantize = FIQ_WUQUANT, int PaletteSize = 256, int ReserveSize = 0,
        RGBQUAD* ReservePalette = nullptr) const
    { return FImage(FreeImage_ColorQuantizeEx(imgPtr, quantize, PaletteSize,  ReserveSize, ReservePalette)); }



    static FImage* Allocate(int width, int height, int bpp = 32, unsigned red_mask = 0xff0000, unsigned green_mask = 0xff00, unsigned blue_mask = 0xff)
    { return new FImage(FreeImage_Allocate( width,  height,  bpp,  red_mask,  green_mask,  blue_mask)); }
    bool LoadFromHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle, int flags = 0);
    void FillImage(const FColor& color);
    void AdjustAlphaP32(float percent);
    FPalette& getPalette(FPalette& palette) const;
    unsigned setPalette(const FPalette& palette);

    void DrawRectangleI8(const FBrush& brush, unsigned x1, unsigned y1, unsigned x2, unsigned y2);
    void DrawRectangleP32(const FBrush& brush, unsigned x1, unsigned y1, unsigned x2, unsigned y2);

};
