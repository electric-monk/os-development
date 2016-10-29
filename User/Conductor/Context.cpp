//
//  Context.cpp
//  TestOSGraphics
//
//  Created by Colin David Munro on 15/09/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#include "Context.h"

namespace Graphics {
    
    static int PixelSize(Bitmap::Format format)
    {
        switch (format) {
            case Bitmap::Format24RGB:
            case Bitmap::Format24BGR:
                return 3;
            case Bitmap::Format32RGBx:
            case Bitmap::Format32BGRx:
            case Bitmap::Format32RGBA:
                return 4;
        }
    }

    UInt32 FrameBuffer::PixelSize(void) const
    {
        return ::Graphics::PixelSize(Type());
    }

    Bitmap::Bitmap(UInt32 width, UInt32 height, Format format, const UInt8 *data)
    :_width(width), _height(height), _format(format)
    {
        UInt32 memSize = width * height * ::Graphics::PixelSize(format);
        _data = new UInt8[memSize];
        if (data)
            CopyMemory(_data, data, memSize);
    }
    
    Bitmap::Bitmap(const Bitmap &other)
    :_width(other._width), _height(other._height), _format(other._format)
    {
        UInt32 memSize = _width * _height * ::Graphics::PixelSize(_format);
        _data = new UInt8[memSize];
        CopyMemory(_data, other._data, memSize);
    }

    Bitmap::~Bitmap()
    {
        delete[] _data;
    }
    
    Bitmap::Format Bitmap::Type(void) const
    {
        return _format;
    }
    UInt32 Bitmap::Width(void) const
    {
        return _width;
    }
    UInt32 Bitmap::Height(void) const
    {
        return _height;
    }
    UInt8* Bitmap::Buffer(void) const
    {
        return _data;
    }
    
    const Colour Colour::White = {0xff, 0xff, 0xff, 0xff};
    const Colour Colour::Black = {0x00, 0x00, 0x00, 0xff};

    // Pixel readers/writers
    
    class F24RGB {
    public:
        static inline Colour Read(UInt8 *pixel)
        {
            return (Colour){pixel[0], pixel[1], pixel[2], 255};
        }
        static inline void Write(UInt8 *pixel, const Colour &value)
        {
            pixel[0] = value.red;
            pixel[1] = value.green;
            pixel[2] = value.blue;
        }
        static inline int Size(void)
        {
            return 3;
        }
    };
    class F24BGR {
    public:
        static inline Colour Read(UInt8 *pixel)
        {
            return (Colour){pixel[2], pixel[1], pixel[0], 255};
        }
        static inline void Write(UInt8 *pixel, const Colour &value)
        {
            pixel[2] = value.red;
            pixel[1] = value.green;
            pixel[0] = value.blue;
        }
        static inline int Size(void)
        {
            return 3;
        }
    };
    class F32RGBx {
    public:
        static inline Colour Read(UInt8 *pixel)
        {
            return F24RGB::Read(pixel);
        }
        static inline void Write(UInt8 *pixel, const Colour &value)
        {
            F24RGB::Write(pixel, value);
            pixel[3] = 255;
        }
        static inline int Size(void)
        {
            return 4;
        }
    };
    class F32BGRx {
    public:
        static inline Colour Read(UInt8 *pixel)
        {
            return F24BGR::Read(pixel);
        }
        static inline void Write(UInt8 *pixel, const Colour &value)
        {
            F24BGR::Write(pixel, value);
            pixel[3] = 255;
        }
        static inline int Size(void)
        {
            return 4;
        }
    };
    class F32RGBA {
    public:
        static inline Colour Read(UInt8 *pixel)
        {
            Colour result = F24RGB::Read(pixel);
            result.alpha = pixel[3];
            return result;
        }
        static inline void Write(UInt8 *pixel, const Colour &value)
        {
            F24RGB::Write(pixel, value);
            pixel[3] = value.alpha;
        }
        static inline int Size(void)
        {
            return 4;
        }
    };
    
    // Drawing functions
    
    template<class Access> class PixelReader
    {
    private:
        UInt8 *_data;
        UInt32 _width;
        UInt32 _height;
        
        UInt32 _deadData;
        
    protected:
        UInt8* Find(UInt32 x, UInt32 y) const
        {
            if ((x >= _width) || (y >= _height)) {
                return (UInt8*)&_deadData;
            }
            return _data + (((_width * y) + x) * Access::Size());
        }
        
    public:
        PixelReader(FrameBuffer &buffer)
        :_data(buffer.Buffer()), _width(buffer.Width()), _height(buffer.Height()), _deadData(0)
        {
        }

        const Colour Get(UInt32 x, UInt32 y) const
        {
            return Access::Read(Find(x, y));
        }
    };
    template<class Access, class Source> class PixelHelper : public PixelReader<Access>
    {
    private:
        const Source& _source;
        
    public:
        PixelHelper(FrameBuffer &buffer, const Source &source)
        :PixelReader<Access>(buffer), _source(source)
        {
        }
        
        void Plot(UInt32 x, UInt32 y)
        {
            Colour colour = _source.Read(x, y);
            
            if (colour.alpha == 0x00)
                return;
            
            Colour output;
            UInt8 *pixel = PixelReader<Access>::Find(x, y);
            if (colour.alpha != 0xFF) {
                Colour background = Access::Read(pixel);
                output.red = UInt8((UInt32(colour.red) * UInt32(colour.alpha) / 255) + (UInt32(background.red) * UInt32(background.alpha) * (255 - colour.alpha)) / (255 * 255));
                output.green = UInt8((UInt32(colour.green) * UInt32(colour.alpha) / 255) + (UInt32(background.green) * UInt32(background.alpha) * (255 - colour.alpha)) / (255 * 255));
                output.blue = UInt8((UInt32(colour.blue) * UInt32(colour.alpha) / 255) + (UInt32(background.blue) * UInt32(background.alpha) * (255 - colour.alpha)) / (255 * 255));
                output.alpha = UInt8(UInt32(colour.alpha) + (UInt32(background.alpha) * (255 - colour.alpha)) / 255);
            } else {
                output = colour;
            }
            Access::Write(pixel, output);
        }
    };
    
    static void ComputePolygonLine(Library::Array<int> &nodeX, const Path &path, Unit y)
    {
        Path::Entry *previous = (Path::Entry*)&(*(path.End() - 1));
        Library::ForEach(path, [&](const Path::Entry &entry){
            if (   ((entry._point.y < y)
                    &&  (previous->_point.y >= y))
                || ((previous->_point.y < y)
                    &&  (entry._point.y >= y))) {
                    nodeX.Add(entry._point.x + (y - entry._point.y) / (previous->_point.y - entry._point.y) * (previous->_point.x - entry._point.x));
                }
            previous = (Path::Entry*)&entry;
            return true;
        });
        Library::Sort(nodeX.Start(), nodeX.End(), [](int &a, int &b){
            return a < b;
        });
    }
    
    static Library::Array<int> CombinePolygonLines(const Library::Array<int> &first, const Library::Array<int> &second)
    {
        Library::Array<int> result;
        
        for (auto it = first.Start(); it != first.End();) {
            // Read values
            int a1 = *it;
            it++;
            int a2 = *it;
            it++;
            // Search in second set
            for (auto jt = second.Start(); jt != second.End();) {
                // Read values
                int b1 = *jt;
                jt++;
                int b2 = *jt;
                jt++;
                // See if we're out of range
                if (b1 > a2)
                    break;
                if (b2 < a1)
                    continue;
                // Add it apparently
                result.Add((a1 > b1) ? a1 : b1);
                result.Add((a2 > b2) ? b2 : a2);
            }
        }
        return result;
    }
    
    template<class Filler, class Source> void FillPolygon(Filler &output, Source &source, const State &state, const Path &path)
    {
        Rect2D bounds = path.Bounds().Intersection(state.clipping.Bounds());
        source.SetBounds(bounds);
//        int leftX = bounds.topLeft.x, rightX = bounds.bottomRight.x;
        int topY = bounds.topLeft.y, bottomY = bounds.bottomRight.y;
        for (int y = topY; y < bottomY; y++) {
            Library::Array<int> nodeX;
            ComputePolygonLine(nodeX, path, y);
            nodeX = CombinePolygonLines(nodeX, state.clipping.Clippings()[y - topY]);
            for (auto it = nodeX.Start(); it != nodeX.End();) {
                // Read values
                int x1 = *it;
                it++;
                int x2 = *it;
                it++;
                // Draw line
                for (int x = x1; x < x2; x++)
                    output.Plot(x, y);
            }
        }
    }
    
    Clipping::Clipping(const Path &path, const Clipping *parent)
    {
        _bounds = path.Bounds();
        Rect2D otherBounds;
        if (parent) {
            otherBounds = parent->Bounds();
            _bounds = otherBounds.Intersection(_bounds);
        }
        int y0 = _bounds.topLeft.y;
        int y1 = _bounds.bottomRight.y;
        for (int y = y0; y < y1; y++) {
            Library::Array<int> nodeX;
            ComputePolygonLine(nodeX, path, y);
            if (parent)
                nodeX = CombinePolygonLines(nodeX, parent->_clippings[y - parent->_bounds.topLeft.y]);
            _clippings.Add(nodeX);
        }
        _bounds.topLeft.y = y0;
        _bounds.bottomRight.y = y1;
    }
    
    Clipping::Clipping(const Clipping &other)
    :_bounds(other._bounds), _clippings(other._clippings)
    {
    }
    
    class GradientColour
    {
    private:
        Colour _first, _second;
        Rect2D _size;
        Matrix2D _matrix;
    public:
        GradientColour(const Colour &first, const Colour &second)
        :_first(first), _second(second)
        {
        }
        void SetBounds(const Rect2D &rect)
        {
            _size = rect;
            Rect2D adjustedSize;
            adjustedSize.topLeft = _matrix.Apply(_size.topLeft);
            adjustedSize.bottomRight = _matrix.Apply(_size.bottomRight);
        }
        void SetMatrix(const Matrix2D &rect)
        {
            _matrix = rect;
        }
        const Colour Read(UInt32 x, UInt32 y) const
        {
            Rect2D adjustedSize;
            adjustedSize.topLeft = _matrix.Apply(_size.topLeft);
            adjustedSize.bottomRight = _matrix.Apply(_size.bottomRight);
            Point2D read = _matrix.Apply((Point2D){Unit(x), Unit(y)});
            Unit position = read.y / 100;
            if (position < 0) position = 0;
            else if (position > 1) position = 1;
            UInt32 ip = UInt32(position * 255);
            UInt32 iip = 255 - ip;
            return (Colour){
                (UInt8)(((_first.red * iip) + (_second.red * ip)) / 255),
                (UInt8)(((_first.green * iip) + (_second.green * ip)) / 255),
                (UInt8)(((_first.blue * iip) + (_second.blue * ip)) / 255),
                (UInt8)(((_first.alpha * iip) + (_second.alpha * ip)) / 255)
            };
        }
    };
    
    template<class Source> void DrawSomething(FrameBuffer &target, const State &state, const Path &path, Source &source)
    {
        source.SetMatrix(state.matrix.Invert());
        Path transformedPath = path.Apply(state.matrix);
        switch (target.Type()) {
            case FrameBuffer::Format24RGB:
            {
                PixelHelper<F24RGB, Source> helper(target, source);
                FillPolygon(helper, source, state, transformedPath);
            }
                break;
            case FrameBuffer::Format24BGR:
            {
                PixelHelper<F24BGR, Source> helper(target, source);
                FillPolygon(helper, source, state, transformedPath);
            }
                break;
            case FrameBuffer::Format32RGBA:
            {
                PixelHelper<F32RGBA, Source> helper(target, source);
                FillPolygon(helper, source, state, transformedPath);
            }
                break;
            case FrameBuffer::Format32RGBx:
            {
                PixelHelper<F32RGBx, Source> helper(target, source);
                FillPolygon(helper, source, state, transformedPath);
            }
                break;
            case FrameBuffer::Format32BGRx:
            {
                PixelHelper<F32BGRx, Source> helper(target, source);
                FillPolygon(helper, source, state, transformedPath);
            }
                break;
        }
    }
    
    class ConstantColour
    {
    private:
        Colour _colour;
    public:
        ConstantColour(const Colour &colour)
        :_colour(colour)
        {
        }
        void SetBounds(const Rect2D &rect)
        {
            // We don't use this
        }
        void SetMatrix(const Matrix2D &rect)
        {
            // We don't use this
        }
        const Colour& Read(UInt32 x, UInt32 y) const
        {
            return _colour;
        }
    };
    void Context::DrawPolygon(const Path &path, const Colour &colour)
    {
        ConstantColour helper(colour);
        DrawSomething(_target, _states[0], path, helper);
    }

    template<class Source> class BitmapDrawer
    {
    private:
        PixelReader<Source> _reader;
        Matrix2D _matrix;
        
    public:
        BitmapDrawer(const FrameBuffer &bitmap)
        :_reader((FrameBuffer&)bitmap)
        {
        }
        void SetBounds(const Rect2D &rect)
        {
            // We don't use this
        }
        void SetMatrix(const Matrix2D &rect)
        {
            _matrix = rect;
        }
        const Colour Read(UInt32 x, UInt32 y) const
        {
            Point2D point = _matrix.Apply((Point2D){Unit(x), Unit(y)});
            // TODO: Blend?
            return _reader.Get(point.x, point.y);
        }
    };
    void Context::DrawBitmap(const Path &path, const FrameBuffer &bitmap)
    {
        switch (bitmap.Type()) {
            case FrameBuffer::Format24RGB:
            {
                BitmapDrawer<F24RGB> reader(bitmap);
                DrawSomething(_target, _states[0], path, reader);
            }
                break;
            case FrameBuffer::Format32RGBA:
            {
                BitmapDrawer<F32RGBA> reader(bitmap);
                DrawSomething(_target, _states[0], path, reader);
            }
                break;
            case FrameBuffer::Format32RGBx:
            {
                BitmapDrawer<F32RGBx> reader(bitmap);
                DrawSomething(_target, _states[0], path, reader);
            }
                break;
            case FrameBuffer::Format32BGRx:
            {
                BitmapDrawer<F32BGRx> reader(bitmap);
                DrawSomething(_target, _states[0], path, reader);
            }
                break;
            case FrameBuffer::Format24BGR:
            {
                BitmapDrawer<F24BGR> reader(bitmap);
                DrawSomething(_target, _states[0], path, reader);
            }
                break;
        }
    }
    void Context::DrawBitmap(const FrameBuffer &bitmap)
    {
        Graphics::Path temp((Graphics::Point2D){0, 0});
        temp.LineTo((Graphics::Point2D){Graphics::Unit(bitmap.Width()), 0});
        temp.LineTo((Graphics::Point2D){Graphics::Unit(bitmap.Width()), Graphics::Unit(bitmap.Height())});
        temp.LineTo((Graphics::Point2D){0, Graphics::Unit(bitmap.Height())});
        DrawBitmap(temp, bitmap);
    }
    
    static void LineRecurse(Path &result, Library::Array<Path::Entry>::ConstIterator start, Library::Array<Path::Entry>::ConstIterator end, const Path::Entry &previous, Unit width)
    {
//        if (start == end)
//            return;
//        const Path::Entry &current = *start;
//        Unit xOffset, yOffset;
//        if (current._point.x == previous._point.x) {
//            xOffset = 0;
//            yOffset = width;
//        } else if (current._point.y == previous._point.y) {
//            xOffset = width;
//            yOffset = 0;
//        } else {
//            // Compute line
//            Unit m = (current._point.x - previous._point.x) / (current._point.y - previous._point.y);
//            // Compute perpendicular line
//            Unit n = -1 / m;
//            xOffset = width / sqrt((n * n) + 1);
//            yOffset = xOffset * n;
//        }
//        // Add left point
//        result.LineTo((Point2D){previous._point.x - xOffset, previous._point.y - yOffset});
//        result.LineTo((Point2D){current._point.x - xOffset, current._point.y - yOffset});
//        // Recurse
//        LineRecurse(result, start + 1, end, current, width);
//        // Add right point
//        result.LineTo((Point2D){current._point.x + xOffset, current._point.y + yOffset});
//        result.LineTo((Point2D){previous._point.x + xOffset, previous._point.y + yOffset});
        if (start == end)
            return;
        const Path::Entry &current = *start;
        Unit angle = Library::Maths::ATan2(current._point.y - previous._point.y, current._point.x - previous._point.x);
        Unit leftAngle = angle - (Library::Maths::Pi / 2);
        Unit rightAngle = angle + (Library::Maths::Pi / 2);
        // Add left point
        Unit leftX = width * Library::Maths::Cosine(leftAngle);
        Unit leftY = width * Library::Maths::Sine(leftAngle);
        result.LineTo((Point2D){leftX + previous._point.x, leftY + previous._point.y});
        result.LineTo((Point2D){leftX + current._point.x, leftY + current._point.y});
        // Recurse
        LineRecurse(result, start + 1, end, current, width);
        // Add right point
        Unit rightX = width * Library::Maths::Cosine(rightAngle);
        Unit rightY = width * Library::Maths::Sine(rightAngle);
        result.LineTo((Point2D){rightX + current._point.x, rightY + current._point.y});
        result.LineTo((Point2D){rightX + previous._point.x, rightY + previous._point.y});
    }
    
    void Context::DrawLine(const Path &path, const Colour &colour, Unit width)
    {
        auto start = path.Start();
        auto end = path.End();
        if (start == end)
            return;
        Path::Entry previous = *start;
        Path result(previous._point);
        LineRecurse(result, start + 1, end, previous, width / 2);
        DrawPolygon(result, colour);
    }

}
