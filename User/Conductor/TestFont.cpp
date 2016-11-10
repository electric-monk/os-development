//
//  TestFont.cpp
//  TestOSGraphics
//
//  Created by Colin David Munro on 25/10/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#include "TestFont.hpp"
namespace ZeppFont {
#include "kernel.font.h"
}

TestFont::TestFont()
{
    for (int i = 0; i < (sizeof(ZeppFont::font) / sizeof(ZeppFont::font[0])); i++) {
        unsigned char *bitmap = ZeppFont::bitmap + (ZeppFont::font[i].offset * ZeppFont::height);
        int charWidth = ZeppFont::font[i].width;

        Graphics::Bitmap newBitmap(charWidth, ZeppFont::height, Graphics::Bitmap::Format8GA);
        UInt8* output = (UInt8*)newBitmap.Buffer();
        
        for (int charY = 0; charY < ZeppFont::height; charY++) {
            unsigned char byte;
            // Prime first read
            for (int charX = 0; charX < charWidth; charX++, byte <<= 1) {
                if ((charX % 8) == 0) {
                    byte = *bitmap;
                    bitmap++;
                }
                output[(((charY * charWidth) + charX) * 2) + 0] = 0xFF;
                output[(((charY * charWidth) + charX) * 2) + 1] = (byte & 0x80) ? 0xFF : 0x00;
            }
        }
        
        _glyphs[i] = newBitmap;
    }
}

static void ProcessString(const char *text, UInt32 length, Library::Function<void(char c)> handler)
{
    if (length == 0) {
        while (*text != '\0') {
            handler(*text);
            text++;
        }
    } else {
        while (length) {
            handler(*text);
            text++;
            length--;
        }
    }
}

void TestFont::Render(Graphics::Context &output, const Graphics::Colour &colour, const char *text, UInt32 length)
{
    ProcessString(text, length, [&](char c){
        Graphics::Bitmap &bitmap = _glyphs[c];
        output.DrawBitmapTinted(bitmap, colour);
        output.Translate(bitmap.Width(), 0);
    });
}

Graphics::Point2D TestFont::Size(const char *text, UInt32 length)
{
    Graphics::Point2D result = (Graphics::Point2D){0, Graphics::Unit(ZeppFont::height)};
    ProcessString(text, length, [&](char c){
        Graphics::Bitmap &bitmap = _glyphs[c];
        result.x += bitmap.Width();
    });
    return result;
}
