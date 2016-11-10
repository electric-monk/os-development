//
//  TestFont.hpp
//  TestOSGraphics
//
//  Created by Colin David Munro on 25/10/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#ifndef TestFont_hpp
#define TestFont_hpp

#include "Context.h"

class TestFont
{
public:
    TestFont();
    
    void Render(Graphics::Context &output, const Graphics::Colour &colour, const char *text, UInt32 length = 0);
    Graphics::Point2D Size(const char *text, UInt32 length = 0);
    
private:
    Library::Dictionary<char, Graphics::Bitmap> _glyphs;
};

#endif /* TestFont_hpp */
