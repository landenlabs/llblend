//-------------------------------------------------------------------------------------------------
//  File: BlendFUtil.cpp
//  Desc: Image blend using FreeImage.
//
//  BlendFUtil created by Dennis Lang on 12/21/21.
//  Copyright © 2021 Dennis Lang. All rights reserved.
//
//  FreeImage Design and implementation by
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

#include "ll_stdhdr.hpp"

#include "blendfutil.hpp"
#include "fprint.hpp"
#include "fbrush.hpp"
#include "fileutil.hpp"
#include "commands.hpp"
#include "directory.hpp"

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
#include <memory>   // unique_ptr


bool BlendFUtil::initDone = false;

// -------------------------------------------------------------------------------------------------
unsigned DLL_CALLCONV
myReadProc(void* buffer, unsigned size, unsigned count, fi_handle handle) {
    return fread(buffer, size, count, (FILE*)handle);
}

unsigned DLL_CALLCONV
myWriteProc(void* buffer, unsigned size, unsigned count, fi_handle handle) {
    return fwrite(buffer, size, count, (FILE*)handle);
}

int DLL_CALLCONV
mySeekProc(fi_handle handle, long offset, int origin) {
    return fseek((FILE*)handle, offset, origin);
}

long DLL_CALLCONV
myTellProc(fi_handle handle) {
    return ftell((FILE*)handle);
}

// -------------------------------------------------------------------------------------------------
FImage& BlendFUtil::LoadImage(FImage& img, const char* fullname) {
    FILE* file = fopen(fullname, "rb");

    if (file != NULL) {
        BlendFUtil::init();

        FreeImageIO io;
        io.read_proc = myReadProc;
        io.write_proc = myWriteProc;
        io.seek_proc = mySeekProc;
        io.tell_proc = myTellProc;

        // find the buffer format
        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)file, 0);

        const char* fmtStr = FreeImage_GetFIFDescription(fif);
        // std::cout << "File format=" << fmtStr << std::endl;

        if (fif != FIF_UNKNOWN) {
            // load from the file handle
            img.LoadFromHandle(fif, &io, (fi_handle)file, 0);
        } else {
            std::cerr << "Failed to load " << fullname << std::endl;
        }
    }
    return img;
}

// -------------------------------------------------------------------------------------------------
bool BlendFUtil::saveTo(const FImage& out, const char* toName) {
    bool okay = false;

    // Get output format from the file name or file extension
    FREE_IMAGE_FORMAT out_fif = FreeImage_GetFIFFromFilename(toName);

    if (out_fif != FIF_UNKNOWN) {
        if ((okay = FreeImage_Save(out_fif, out.imgPtr, toName, 0)))
            std::cout << "Saved to " << toName << std::endl;
        else
            std::cerr << "Saved FAILED to " << toName << std::endl;
    }

    return okay;
}

// -------------------------------------------------------------------------------------------------
// Index paletized image mapping from src palette to dst palette.
unsigned BlendFUtil::BestMapping(
    const FPalette& srcPalette,
    const FPalette& dstPalette,
    const BYTE* dstMapping,
    Mapping& mappings) {
    mappings.reset();

    for (unsigned srcIdx = 0; srcIdx < srcPalette.size(); srcIdx++) {
        const FColor& srcColor = srcPalette[srcIdx];
        unsigned bestIdx = dstPalette.findClosest(srcColor);
        if (bestIdx == 256) {
            bestIdx = dstPalette.findAlpha(srcColor);
        }
        mappings.to[srcIdx] = dstMapping[bestIdx];
    }

    if (false) {
        for (unsigned srcIdx = 0; srcIdx < srcPalette.size(); srcIdx++) {
            unsigned dstIdx = mappings.to[srcIdx];
            printf("%3d: %s -> ", srcIdx, FPrint::toString(srcPalette[srcIdx]));
            printf("%3d: %s\n", dstIdx, FPrint::toString(dstPalette[dstIdx]));
        }
    }

    return 0;
}

// -------------------------------------------------------------------------------------------------
// Truecolor 32bit blend,  top is blended over bottom.
FImage& BlendFUtil::BlendP32(const FImage& topImgP32, FImage& botImgP32) {
    unsigned botImgBPP = botImgP32.GetBitsPerPixel();
    if (botImgBPP != 32) {
        std::cerr << "Blend - Bottom image not 32bit" << std::endl;
        return botImgP32;
    }

    unsigned topImgBPP = topImgP32.GetBitsPerPixel();
    if (topImgBPP != 32) {
        std::cerr << "Blend - Top image not 32bit" << std::endl;
        return botImgP32;
    }

    if (sizeof(FColor) != 4) {
        std::cerr << "Bug with color size\n";
    }

    unsigned widthTop = topImgP32.GetWidth();
    unsigned heightTop = topImgP32.GetHeight();
    unsigned widthBot = botImgP32.GetWidth();
    unsigned heightBot = botImgP32.GetHeight();
    unsigned height = min(heightTop, heightBot);
    unsigned width = min(widthTop, widthBot);

    for (unsigned y = 0; y < height; y++) {
        const FColor* top_argb = (const FColor*)topImgP32.ReadScanLine(y);
        FColor* bot_argb = (FColor*)botImgP32.ScanLine(y);

        for (unsigned x = 0; x < width; x++) {
            FColor& botColor = *bot_argb++;
            top_argb[x].blendOver(botColor);
        }
    }

    return botImgP32;
}

// -------------------------------------------------------------------------------------------------
// Index 8bit palette blended over 32bit bottom.
FImage& BlendFUtil::BlendI8_P32(const FPalette& topPalette, const FImage& topImgI8, FImage& botImgP32) {
    unsigned widthTop = topImgI8.GetWidth();
    unsigned heightTop = topImgI8.GetHeight();
    unsigned widthBot = botImgP32.GetWidth();
    unsigned heightBot = botImgP32.GetHeight();

    unsigned height = min(heightTop, heightBot);
    unsigned width = min(widthTop, widthBot);

    for (unsigned y = 0; y < height; y++) {
        const BYTE* top = topImgI8.ReadScanLine(y);
        RGBQUAD* bot = (RGBQUAD*)botImgP32.ScanLine( y);
        for (unsigned x = 0; x < width; x++) {
            const FColor& topColor = topPalette[top[x]];
            topColor.blendOver(bot[x]);
        }
    }

    return botImgP32;
}

// -------------------------------------------------------------------------------------------------
// Output is maximizing pixel index, output = max(input, output)
FImage& BlendFUtil::MaximumI8(const FImage& inImgI8, FImage& outImgI8) {
    unsigned widthIn   = inImgI8.GetWidth();
    unsigned heightIn  = inImgI8.GetHeight();
    unsigned widthOut  = outImgI8.GetWidth();
    unsigned heightOut = outImgI8.GetHeight();

    unsigned height    = min(heightIn, heightOut);
    unsigned width     = min(widthIn, widthOut);

    for (unsigned y = 0; y < height; y++) {
        const BYTE* in = inImgI8.ReadScanLine(y);
        BYTE* out = outImgI8.ScanLine(y);
        for (unsigned x = 0; x < width; x++) {
            out[x] = max(in[x], out[x]);   // Output is maximum pixel index.
        }
    }

    return outImgI8;
}

// -------------------------------------------------------------------------------------------------
void BlendFUtil::Dump(const char* fullname) {
    FImage img;
    if (LoadImage(img, fullname).Valid()) {
        FPrint::printInfo(img, fullname);
        FPrint::printPalette(img);
        FPrint::printHisto(img);
    }
}

// -------------------------------------------------------------------------------------------------
FImageRef& BlendFUtil::Blend(const char* fullname, const BlendCfg& cfg, FImageRef& grayImgP32Ref) {
    FImage imgI8;
    if (LoadImage(imgI8, fullname).Valid()) {

        unsigned bitsPerPixel = imgI8.GetBitsPerPixel();
        if (bitsPerPixel != 8) {
            FPrint::printInfo(imgI8, fullname);
            //    FPrint::printPalette(img);
            //    FPrint::printHisto(img);
            std::cerr << fullname << " must by 8 bit per pixel images\n";
            return grayImgP32Ref;
        }
        unsigned width = imgI8.GetWidth();
        unsigned height = imgI8.GetHeight();
        unsigned colors = imgI8.GetColorsUsed();

        FImage imgP32 = imgI8.ConvertTo32Bits();
        if (grayImgP32Ref != nullptr) {
            grayImgP32Ref->AdjustAlphaP32(0.99f);
            BlendFUtil::BlendP32(*grayImgP32Ref, imgP32);
        }
        lstring fullPath(fullname);
        lstring outFname;
        FileUtil::getName(outFname, fullPath);
        BlendFUtil::saveTo(imgP32, outFname);
        imgP32.Close();

        FPalette imgPalette;
        imgI8.getPalette(imgPalette);

        /*
        const FPalette& nowradPalette = FPalette::getNowradPalette();
        const FPalette& overlayPalette = FPalette::getNowradGrayPalette();
        const Mapping& nMapping = FPalette::getNowradToGrayMapping();
        Mapping mapping;
         BlendFUtil::BestMapping(imgPalette, nowradPalette, nMapping.to, mapping);
        */

#if 0
        const Mapping& mapping = cfg.getMapping();
        const FPalette& overlayPalette = cfg.getOverlayPalette();

        unsigned changed;
        changed = imgI8.ApplyPaletteIndexMapping(mapping.from, mapping.to, colors, false);
        imgI8.setPalette(overlayPalette);

        if (grayImgP32Ref == nullptr) {
            FImage* imgPtr = FImage::Allocate(width, height, 32, 0xff0000, 0xf00, 0xff);
            FImageRef imgRef(imgPtr);
            grayImgP32Ref.swap(imgRef);
            grayImgP32Ref->FillImage(FPalette::TRANSPARENT);
        }

        BlendI8_P32(overlayPalette, imgI8, grayImgP32Ref);
#endif

        imgI8.Close();
    }

    return grayImgP32Ref;
}

// -------------------------------------------------------------------------------------------------
void BlendFUtil::Palette(const char* fullname) {
    BlendFUtil::init();

    FImage imgI8;
    if (! LoadImage(imgI8, fullname).Valid()) {
        std::cerr << "Paltette - Failed to load " << fullname << std::endl;
        return;
    }

    FPalette palette;
    imgI8.getPalette(palette);

    const unsigned BOX_CNT = 32;
    const unsigned BOX_WIDTH = 32;
    const unsigned BOX_HEIGHT = 32;

    if (true) {
        FImageRef paletteImgRef(FImage::Allocate(BOX_WIDTH * BOX_CNT, BOX_HEIGHT, 8));
        paletteImgRef->SetBackgroundColor(palette[0]);

        FBrush brush;
        for (unsigned boxIdx = 0; boxIdx < BOX_CNT; boxIdx++) {
            brush.fillIndex = boxIdx;
            brush.lineIndex = palette.findColor(FPalette::GRAY);
            brush.lineWidth = 2;

            unsigned x1 = boxIdx * BOX_WIDTH;
            unsigned y1 = 0;
            paletteImgRef->DrawRectangleI8(brush, x1, y1, x1 + BOX_WIDTH, y1 + BOX_HEIGHT);
        }

        paletteImgRef->SetTransparent(true);
        paletteImgRef->SetTransparentIndex(0);
        paletteImgRef->setPalette(palette);

        // FPrint::printInfo(*paletteImgRef, "Palette (horizontal)");
        // FPrint::printPalette(*paletteImgRef, BOX_WIDTH);
        // FPrint::printHisto(*paletteImgRef, BOX_WIDTH);

        BlendFUtil::saveTo(*paletteImgRef, "/tmp/paletteH.png");
    }

    if (true) {
        FImageRef paletteImgRef(FImage::Allocate(BOX_WIDTH, BOX_HEIGHT * BOX_CNT, 8));
        paletteImgRef->SetBackgroundColor(palette[0]);

        FBrush brush;
        for (unsigned boxIdx = 0; boxIdx < BOX_CNT; boxIdx++) {
            brush.fillIndex = boxIdx;
            brush.lineIndex = palette.findColor(FPalette::GRAY);
            brush.lineWidth = 2;

            unsigned x1 = 0;
            unsigned y1 = boxIdx * BOX_HEIGHT;
            paletteImgRef->DrawRectangleI8(brush, x1, y1, x1 + BOX_WIDTH, y1 + BOX_HEIGHT);
        }

        paletteImgRef->SetTransparent(true);
        paletteImgRef->SetTransparentIndex(0);
        paletteImgRef->setPalette(palette);

        // FPrint::printInfo(*paletteImgRef, "Palette (vertical)");
        // FPrint::printPalette(*paletteImgRef, BOX_WIDTH);
        // FPrint::printHisto(*paletteImgRef, BOX_WIDTH);

        BlendFUtil::saveTo(*paletteImgRef, "/tmp/paletteV.png");
    }
    /*
        if (false) {
            unsigned bitsPerPixel = paletteImgRef->GetBitsPerPixel();
            if (bitsPerPixel != 24) {
                // Quantize only supports 24 bit images.
                FImage hDIB24bpp = paletteImgRef->ConvertTo24Bits();
                hDIB24bpp.setPalette(rainPalette);  // Not sure if this works on 24 RGB image
                FPrint::printInfo(hDIB24bpp, "convert To 24 Bit");
                BlendFUtil::saveTo(hDIB24bpp, "/tmp/ftest2.png");
            }
        }

        if (false) {
            // Note - Quantize only works on 24 RGB image, lost transparency.
            unsigned colors = (unsigned)rainPalette.size();
            FImage hDIB8bpp = paletteImgRef->ColorQuantizeEx(FIQ_NNQUANT, colors, colors, rainPalette.quads());
            // BlendFUtil::setPalette(paletteImg, rainPalette);
            FPrint::printInfo(hDIB8bpp, "quantized to palette");
            FPrint::printPalette(hDIB8bpp);
            BlendFUtil::saveTo(hDIB8bpp, "/tmp/ftest3.png");
        }
    */

}
