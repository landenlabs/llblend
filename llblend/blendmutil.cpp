//-------------------------------------------------------------------------------------------------
//  File: BlendMUtil.cpp
//  Desc: Image blend (manipulation) using ImageMagick Magick++ library
//
//. See link for ImageMagick (Magick++) library.
//  http://www.graphicsmagick.org/Magick++/Image.html#map
//
//  BlendMUtil created by Dennis Lang on 12/21/21.
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

#include "BlendMUtil.hpp"

#define MAGICKCORE_QUANTUM_DEPTH 32
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>
// using namespace Magick;

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

#include "FileUtil.hpp"
#include "commands.hpp"
#include "directory.hpp"
#include "ll_stdhdr.hpp"
#include "split.hpp"

ostream& operator<<(ostream& os, const Magick::ColorRGB& color) {
    os << (int)(color.red() * 256)
        << ","
        << (int)(color.green() * 256)
        << ","
        << (int)(color.blue() * 256);
    return os;
}

std::ostream& operator<<(std::ostream& strm, MagickCore::ImageType tt) {
    const string nameTT[] = {
        "UndefinedType",
        "BilevelType",
        "GrayscaleType",
        "GrayscaleAlphaType",
        "PaletteType",
        "PaletteAlphaType",
        "TrueColorType",
        "TrueColorAlphaType",
        "ColorSeparationType",
        "ColorSeparationAlphaType",
        "OptimizeType",
        "PaletteBilevelAlphaType"
    };
    return strm << nameTT[tt];
}

static Magick::Color ToColor(unsigned red, unsigned green, unsigned blue, unsigned alpha) {
    float cMax = 256.0;
    return Magick::ColorRGB(red / cMax, green / cMax, blue / cMax);  // , alpha/cMax);
}

void ImageMUtil::action(const lstring& fullname, DIR_TYPES dtype) {
    const char* palettePath = "/tmp/palette.png";

    lstring name;
    FileUtil::getName(name, fullname);
    Magick::Image* imageRefPalette = nullptr;

    struct stat info;
    if (stat(fullname, &info) == 0 && FileUtil::isWriteableFile(info)) {
        std::cout << fullname.c_str() << std::endl;
    } else {
        std::cout << "ReadOnly " << fullname.c_str() << std::endl;
    }

    std::vector<Magick::Color> palette;
    // palette.push_back(ToColor(99,235,99,0));
    palette.push_back(ToColor(0, 0, 0, 0));
    palette.push_back(ToColor(99, 235, 99, 255));
    palette.push_back(ToColor(60, 199, 60, 255));
    palette.push_back(ToColor(28, 158, 52, 255));
    palette.push_back(ToColor(14, 104, 26, 255));
    // palette.push_back(ToColor(0,63,0,255));
    palette.push_back(ToColor(0, 63, 0, 255));
    palette.push_back(ToColor(251, 235, 2, 255));
    palette.push_back(ToColor(238, 109, 2, 255));
    palette.push_back(ToColor(210, 11, 6, 255));
    palette.push_back(ToColor(189, 8, 4, 255));
    palette.push_back(ToColor(169, 5, 3, 255));
    palette.push_back(ToColor(148, 2, 1, 255));
    palette.push_back(ToColor(128, 0, 0, 255));
    palette.push_back(ToColor(255, 255, 255, 255));
    // palette.push_back(ToColor(255,255,255,255));
    // palette.push_back(ToColor(255,255,255,255));
    // palette.push_back(ToColor(255,255,255,255));

    // http://www.graphicsmagick.org/Magick++/Image.html#map
    const unsigned BOX_WIDTH = 32;
    const unsigned BOX_HEIGHT = 32;

    Magick::Image paletteImg(Magick::Geometry(32 * BOX_WIDTH, BOX_HEIGHT), Magick::ColorRGB(0, 0, 0, 0));
    size_t idx;
    try {
        paletteImg.classType(Magick::PseudoClass);
        cout << "color depth=" << paletteImg.depth() << std::endl;
        paletteImg.depth(8);
        paletteImg.type(MagickCore::PaletteAlphaType);
        paletteImg.colorMapSize(256);
        size_t colorMapSize = paletteImg.colorMapSize();
        std::vector<Magick::Drawable> drawList;

        for (idx = 0; idx < palette.size() && idx < colorMapSize; idx++) {
            paletteImg.colorMap(idx, palette[idx]);
        }
        colorMapSize = paletteImg.colorMapSize();
        cout << "Color map size=" << colorMapSize << std::endl;
        for (idx = 0; idx < palette.size(); idx++) {
            try {
                Magick::Color color = palette[idx];
                int x = (unsigned)idx * BOX_WIDTH;
                int y = 0;
                // paletteImg.pixelColor(x, y, color);
#if 0
                paletteImg.fillColor(color);
                paletteImg.draw( DrawableRectangle(x, y, x + BOX_WIDTH, y + BOX_HEIGHT) );
#else
                drawList.push_back(Magick::DrawableFillColor(color));
                drawList.push_back(Magick::DrawableRectangle(x, y, x + BOX_WIDTH, y + BOX_HEIGHT));
#endif
            } catch (Magick::Exception& ex) {
                cerr << "Caught exception: " << ex.what() << " on color " << idx << endl;
            }
        }

        try {
            colorMapSize = paletteImg.colorMapSize();
            cout << "Color map size=" << colorMapSize << std::endl;
        } catch (Magick::Exception& ex) {
            paletteImg.colorMapSize(256);
            size_t colorMapSize = paletteImg.colorMapSize();
            std::vector<Magick::Drawable> drawList;

            for (idx = 0; idx < palette.size() && idx < colorMapSize; idx++) {
                paletteImg.colorMap(idx, palette[idx]);
            }
            colorMapSize = paletteImg.colorMapSize();
            cout << "Color map size=" << colorMapSize << std::endl;
        }

        // paletteImg.syncPixels();
        Magick::ImageType imageType = paletteImg.type();
        std::cerr << "Palette type=" << imageType << std::endl;
        paletteImg.draw(drawList);
        paletteImg.syncPixels();
        paletteImg.write("/tmp/palette_ref.png");
        colorMapSize = paletteImg.colorMapSize();
        cout << "Color map size=" << colorMapSize << std::endl;
    } catch (Magick::Exception& ex) {
        cerr << "Caught exception: " << ex.what() << " on color " << idx << endl;
    }

    Magick::Image image;

    try {
        // image.type(PaletteType);
        // image.read(fullname);
        image = Magick::Image(fullname);

        image.colorMapSize(256);
        size_t colorMapSize = image.colorMapSize();
        for (idx = 0; idx < palette.size() && idx < colorMapSize; idx++) {
            image.colorMap(idx, palette[idx]);
#if 0
            const unsigned BOX_WIDTH = 20;
            const unsigned BOX_HEIGHT = 20;
            image.fillColor(palette[idx]);
            int x = (unsigned)idx * BOX_WIDTH;
            int y = 0;
            image.draw( DrawableRectangle(x, y, x + BOX_WIDTH, y + BOX_HEIGHT) );
#endif
        }

        image.map(paletteImg, false);
        // image.quantizeColors(palette.size()+1 );
    } catch (Magick::Exception& error_) {
        cerr << "Caught exception: " << error_.what() << endl;
    }

    try {
        Magick::ImageType imageType = image.type();
        std::cerr << "ImageType=" << imageType << " " << fullname << std::endl;

        if (image.classType() == Magick::ClassType::DirectClass) {
            image.type(Magick::PaletteAlphaType);
            // image.quantizeColorSpace( GRAYColorspace );
            // image.quantizeColors( 256 );
            // image.quantize( );
        }

        if (image.classType() == Magick::ClassType::PseudoClass) {
            //  image.colorSpace()
            // image.quantizeColors( palette.size() );
            // image.quantizeColorSpace(GRAYColorspace);
            size_t colorMapSize = image.colorMapSize();

            string outSize = "100x";
            const unsigned BOX_WIDTH = 20;
            const unsigned BOX_HEIGHT = 20;
            const unsigned BOX_MARGIN = 2;
            const unsigned ROW_HEIGHT = BOX_HEIGHT + BOX_MARGIN;
            outSize += std::to_string(colorMapSize * ROW_HEIGHT);
            Magick::Image out(outSize.c_str(), "black");

            // out.modifyImage();
            // Set the image type to TrueColor DirectClass representation.
            // out.type(PaletteType);   // DOES NOT WORK !!!

            out.strokeColor("white");
            out.font("Helvetica");
            // out.font("Arial");
            out.fontPointsize(24);

            std::vector<Magick::Drawable> drawList;
            // drawList.push_back(DrawablePointSize(32));
            // drawList.push_back(DrawableFont("Microsoft Sans Serif", AnyStyle, 32, AnyStretch));
            // drawList.push_back(DrawableFont("/Library/Fonts/Arial.ttf"));     // no problem

            for (idx = 0; idx < colorMapSize; idx++) {
                // if (idx > 7) break;
                Magick::ColorRGB color = image.colorMap(idx);
                std::cout << std::setw(3) << idx << " " << color << std::endl;

                // Draw a rectangle
                size_t row = idx * ROW_HEIGHT;
                string str = to_string(idx);
#if 0
                out.fillColor(color);
                out.draw( DrawableRectangle(0, row, BOX_WIDTH, row + BOX_HEIGHT) );
                out.draw( DrawableText(BOX_WIDTH, row, str));
#else
                drawList.push_back(Magick::DrawableFillColor(color));
                drawList.push_back(Magick::DrawableRectangle(0, row, BOX_WIDTH, row + BOX_HEIGHT));
                drawList.push_back(Magick::DrawableText(BOX_WIDTH, row, str));
#endif
            }

            out.draw(drawList);
            out.syncPixels();

            /*
            image.quantizeColorSpace( GRAYColorspace );
            image.quantizeColors( 256 );
            image.quantize( );
             */
            if (imageRefPalette != nullptr) {
                out.map(*imageRefPalette);
            }

            out.write(palettePath);
        }
    } catch (Magick::Exception& ex) {
        cerr << "Caught exception: " << ex.what() << "   " << idx << endl;
    }
}
