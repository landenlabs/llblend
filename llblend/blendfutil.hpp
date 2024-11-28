//-------------------------------------------------------------------------------------------------
//  File: BlendFUtil.hpp
//  Desc: Image blend using FreeImage.
//
//  BlendFUtil created by Dennis Lang on 12/21/21.
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

#include <iostream>

#include "FImage.hpp"
#include "FPalette.hpp"
#include "FBrush.hpp"
#include "BlendCfg.hpp"

class BlendFUtil {
public:
    static bool saveTo(const FImage& img, const char* toName);
    static FImage& LoadImage(FImage& img, const char* fullname);

    static void FreeImageErrorHandler(FREE_IMAGE_FORMAT imgFmt, const char* message) {
        std::cerr << "\nFreeImage error ";
        if (imgFmt != FIF_UNKNOWN) {
            std::cerr << FreeImage_GetFormatFromFIF(imgFmt);
        }
        std::cerr << message << std::endl;
    }

    static bool initDone;
    static void init() {
        if (! initDone) {
            // Call this ONLY when linking with FreeImage as a static library
            FreeImage_Initialise();

            // initialize your own FreeImage error handler
            FreeImage_SetOutputMessage(FreeImageErrorHandler);

            // Print version & copyright infos
            std::cout << FreeImage_GetVersion() << std::endl << FreeImage_GetCopyrightMessage() << std::endl;

            initDone = true;
        }
    }


    static void Dump(const char* fullname);
    static void Palette(const char* fullname);

    static FImageRef& Blend(const char* fullname, const BlendCfg& cfg, FImageRef& grayImgRef );
    static FImage& BlendP32(const FImage& topImgP32,  FImage& botImgP32);
    static FImage& BlendI8_P32(const FPalette& topPalette, const FImage& topImgI8,  FImage& botImgP32);

    static FImage& MaximumI8(const FImage& inImgI8, FImage& outImgI8);       // out = max(in, out)

    static unsigned BestMapping(const FPalette& srcPalette, const FPalette& dstPalette, const BYTE* dstMapping, Mapping& mappings);

    static void AdjustAlpha(float percent, const FImage& imgP32);
};

