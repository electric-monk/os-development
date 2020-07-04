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
            case Bitmap::Format8GA:
                return 2;
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
            CopyFast(_data, data, memSize);
    }
    
    Bitmap::Bitmap(const Bitmap &other)
    :_width(other._width), _height(other._height), _format(other._format)
    {
        UInt32 memSize = _width * _height * ::Graphics::PixelSize(_format);
        _data = new UInt8[memSize];
        CopyFast(_data, other._data, memSize);
    }

    Bitmap::Bitmap()
    :_width(0), _height(0), _format(Graphics::Bitmap::Format8GA)
    {
        _data = new UInt8[0];
    }

    Bitmap::~Bitmap()
    {
        delete[] _data;
    }
    
    Bitmap& Bitmap::operator =(const Bitmap &other)
    {
        if (this != &other) {
            delete[] _data;
            _width = other._width;
            _height = other._height;
            _format = other._format;
            UInt32 memSize = _width * _height * ::Graphics::PixelSize(_format);
            _data = new UInt8[memSize];
            CopyFast(_data, other._data, memSize);
        }
        return *this;
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
    class F8GA {
    public:
        static inline Colour Read(UInt8 *pixel)
        {
            Colour result;
            result.red = result.green = result.blue = pixel[0];
            result.alpha = pixel[1];
            return result;
        }
        static inline void Write(UInt8 *pixel, const Colour &value)
        {
            pixel[0] = UInt8((UInt32(value.red) + UInt32(value.green) + UInt32(value.blue)) / 3);
            pixel[1] = value.alpha;
        }
        static inline int Size(void)
        {
            return 2;
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
    
    template<typename ArrayType>
    static void ComputePolygonLine(ArrayType &nodeX, const Path &path, Unit y)
    {
        Path::Entry *previous = (Path::Entry*)&(*(path.end() - 1));
        nodeX.PrepareFor(path.end() - path.begin());
        for (const auto& entry : path) {
            if (   (((entry._point.y <= y)
                    &&  (previous->_point.y >= y))
                || ((previous->_point.y <= y)
                    &&  (entry._point.y >= y)))
                && (entry._point.y != previous->_point.y)) {
                    nodeX.Add(entry._point.x + (y - entry._point.y) / (previous->_point.y - entry._point.y) * (previous->_point.x - entry._point.x));
                }
            previous = (Path::Entry*)&entry;
        }
        Library::Sort(nodeX.begin(), nodeX.end(), [](int &a, int &b){
            return a < b;
        });
    }
    
    template<typename Type>
    class StaticArray
    {
    private:
        Type *_array;
        UInt32 _cursor;
        UInt32 _length;
    public:
        StaticArray(Type *array, unsigned long size)
        :_array(array), _length(UInt32(size) / sizeof(Type)), _cursor(0)
        {}
        
        using Iterator = Type*;
        using ConstIterator = const Type*;
        
        Iterator begin(void) { return _array; }
        Iterator end(void) { return _array + _cursor; }
        const Iterator begin(void) const { return _array; }
        const Iterator end(void) const { return _array + _cursor; }
        Type& operator[](int index) { return _array[index]; }
        void PrepareFor(long) {}
        void Add(const Type& v) { _array[_cursor++] = v; }
    };
    
    template<typename ArrayTypeFirst, typename ArrayTypeSecond>
    class PolygonLinesCombiner
    {
    private:
        const ArrayTypeFirst &_first;
        const ArrayTypeSecond &_second;
    public:
        struct Match {
            int x1, x2;
        };
        class Iterator
        {
        private:
            const PolygonLinesCombiner& _owner;
            typename ArrayTypeFirst::ConstIterator _outer;
            typename ArrayTypeSecond::ConstIterator _inner;
            int _a1, _a2, _b1, _b2;
            bool _done;
            void Step(void)
            {
                while (true) {
                    while (_inner != _owner._second.end()) {
                        _b1 = *_inner;
                        _inner++;
                        _b2 = *_inner;
                        _inner++;
                        // See if we're out of range
                        if (_b1 > _a2)
                            break;
                        if (_b2 < _a1)
                            continue;
                        // Add it apparently
                        return;
                    }
                    if (_outer == _owner._first.end())
                        break;
                    _inner = _owner._second.begin();
                    _a1 = *_outer;
                    _outer++;
                    _a2 = *_outer;
                    _outer++;
                }
                _done = true;
            }
        public:
            Iterator(const PolygonLinesCombiner& owner, bool end)
            :_owner(owner), _outer(end ? owner._first.end() : owner._first.begin()), _inner(owner._second.end()), _done(false)
            {
                Step();
            }
            
            Match operator*() const
            {
                return {Library::Max(_a1, _b1), Library::Min(_a2, _b2)};
            }
            
            Iterator& operator++()
            {
                Step();
                return *this;
            }
            
            bool operator!=(const Iterator& other)
            {
                return (_outer != other._outer) || (_inner != other._inner) || (_done != other._done);
            }
        };
        
        PolygonLinesCombiner(const ArrayTypeFirst &first, const ArrayTypeSecond &second)
        :_first(first), _second(second)
        {
        }
        
        Iterator begin() const
        {
            return Iterator(*this, false);
        }
        
        Iterator end() const
        {
            return Iterator(*this, true);
        }
    };
    
    template<class Filler, class Source> void FillPolygon(Filler &output, Source &source, const State &state, const Path &path)
    {
        Rect2D bounds = path.Bounds().Intersection(state.clipping.Bounds());
        source.SetBounds(bounds);
//        int leftX = bounds.topLeft.x, rightX = bounds.bottomRight.x;
        int topY = bounds.topLeft.y, bottomY = bounds.bottomRight.y;
        for (int y = topY; y <= bottomY; y++) {
            int nodeData[path.end() - path.begin()];
            StaticArray<int> nodeX(nodeData, sizeof(nodeData));
            ComputePolygonLine(nodeX, path, y);
            PolygonLinesCombiner<StaticArray<int>, Library::Array<int>> combiner(nodeX, state.clipping.Clippings()[y - topY]);
            for (auto chunk : combiner) {
                // Draw line
                for (int x = chunk.x1; x < chunk.x2; x++)
                    output.Plot(x, y);
            }
        }
    }
    
    Clipping::Clipping(const Path &path, const Clipping *parent)
    :_clippings(new Library::Array<Library::Array<int>>())
    {
        _bounds = path.Bounds();
        Rect2D otherBounds;
        if (parent) {
            otherBounds = parent->Bounds();
            _bounds = otherBounds.Intersection(_bounds);
        }
        int y0 = _bounds.topLeft.y;
        int y1 = _bounds.bottomRight.y;
        _clippings->PrepareFor(1 + y1 - y0);
        for (int y = y0; y <= y1; y++) {
            Library::Array<int> nodeX;
            ComputePolygonLine(nodeX, path, y);
            if (parent) {
                int offset = y - parent->_bounds.topLeft.y;
                if ((offset >= 0) && (offset < parent->_clippings->Count())) {
                    Library::Array<int> replacement;
                    replacement.PrepareFor(1 + y1 - y0);
                    PolygonLinesCombiner<Library::Array<int>, Library::Array<int>> combiner(nodeX, (*parent->_clippings)[offset]);
                    for (auto chunk : combiner) {
                        replacement.Add(chunk.x1);
                        replacement.Add(chunk.x2);
                    }
                    _clippings->Add(replacement);
                } else {
                    _clippings->Add(Library::Array<int>());
                }
            } else {
                _clippings->Add(nodeX);
            }
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
            case FrameBuffer::Format8GA:
            {
                PixelHelper<F8GA, Source> helper(target, source);
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
        DrawSomething(_target, CurrentState(), path, helper);
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
        State &state = CurrentState();
        switch (bitmap.Type()) {
            case FrameBuffer::Format24RGB:
            {
                BitmapDrawer<F24RGB> reader(bitmap);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format32RGBA:
            {
                BitmapDrawer<F32RGBA> reader(bitmap);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format32RGBx:
            {
                BitmapDrawer<F32RGBx> reader(bitmap);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format32BGRx:
            {
                BitmapDrawer<F32BGRx> reader(bitmap);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format24BGR:
            {
                BitmapDrawer<F24BGR> reader(bitmap);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format8GA:
            {
                BitmapDrawer<F8GA> reader(bitmap);
                DrawSomething(_target, state, path, reader);
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

    template<class Source> class BitmapTinter : public BitmapDrawer<Source>
    {
    public:
        BitmapTinter(const FrameBuffer &bitmap, const Colour &tint)
        :BitmapDrawer<Source>(bitmap), _colour(tint)
        {
        }
        const Colour Read(UInt32 x, UInt32 y) const
        {
            Colour temp = BitmapDrawer<Source>::Read(x, y);
            return (Colour){
                UInt8((UInt32(temp.red) * UInt32(_colour.red)) / 255),
                UInt8((UInt32(temp.green) * UInt32(_colour.green)) / 255),
                UInt8((UInt32(temp.blue) * UInt32(_colour.blue)) / 255),
                UInt8((UInt32(temp.alpha) * UInt32(_colour.alpha)) / 255)
            };
        }
    private:
        Colour _colour;
    };
    void Context::DrawBitmapTinted(const Path &path, const FrameBuffer &bitmap, const Colour &tint)
    {
        State &state = CurrentState();
        switch (bitmap.Type()) {
            case FrameBuffer::Format24RGB:
            {
                BitmapTinter<F24RGB> reader(bitmap, tint);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format32RGBA:
            {
                BitmapTinter<F32RGBA> reader(bitmap, tint);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format32RGBx:
            {
                BitmapTinter<F32RGBx> reader(bitmap, tint);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format32BGRx:
            {
                BitmapTinter<F32BGRx> reader(bitmap, tint);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format24BGR:
            {
                BitmapTinter<F24BGR> reader(bitmap, tint);
                DrawSomething(_target, state, path, reader);
            }
                break;
            case FrameBuffer::Format8GA:
            {
                BitmapTinter<F8GA> reader(bitmap, tint);
                DrawSomething(_target, state, path, reader);
            }
                break;
        }
    }
    void Context::DrawBitmapTinted(const FrameBuffer &bitmap, const Colour &tint)
    {
        Graphics::Path temp((Graphics::Point2D){0, 0});
        temp.LineTo((Graphics::Point2D){Graphics::Unit(bitmap.Width()), 0});
        temp.LineTo((Graphics::Point2D){Graphics::Unit(bitmap.Width()), Graphics::Unit(bitmap.Height())});
        temp.LineTo((Graphics::Point2D){0, Graphics::Unit(bitmap.Height())});
        DrawBitmapTinted(temp, bitmap, tint);
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
        auto start = path.begin();
        auto end = path.end();
        if (start == end)
            return;
        Path::Entry previous = *start;
        Path result(previous._point);
        LineRecurse(result, start + 1, end, previous, width / 2);
        DrawPolygon(result, colour);
    }

}
