//
//  Context.h
//
//  Created by Colin David Munro on 15/09/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#ifndef Context_hpp
#define Context_hpp

#include "Library.h"
#include "Maths.h"

namespace Graphics {
    typedef double Unit;
    typedef struct { Unit x, y; } Point2D;
    typedef struct Rect2D {
        Point2D topLeft, bottomRight;
        Rect2D Union(const Rect2D &other) const
        {
            Rect2D result = *this;
            if (other.topLeft.x < result.topLeft.x)
                result.topLeft.x = other.topLeft.x;
            if (other.topLeft.y < result.topLeft.y)
                result.topLeft.y = other.topLeft.y;
            if (other.bottomRight.x > result.bottomRight.x)
                result.bottomRight.x = other.bottomRight.x;
            if (other.bottomRight.y > result.bottomRight.y)
                result.bottomRight.y = other.bottomRight.y;
            return result;
        }
        bool Intersects(const Rect2D &other) const
        {
            int x1 = CategoriseEnd(other.topLeft.x, topLeft.x, bottomRight.x);
            int x2 = CategoriseEnd(other.bottomRight.x, topLeft.x, bottomRight.x);
            int y1 = CategoriseEnd(other.topLeft.y, topLeft.y, bottomRight.y);
            int y2 = CategoriseEnd(other.bottomRight.y, topLeft.y, bottomRight.y);
            bool h = (x1 != x2) || (x1 == 0) || (x2 == 0);
            bool v = (y1 != y2) || (y1 == 0) || (y2 == 0);
            return h && v;
        }
        Rect2D Offset(const Point2D &point) const
        {
            Rect2D result;
            result.topLeft.x = topLeft.x + point.x;
            result.topLeft.y = topLeft.y + point.y;
            result.bottomRight.x = bottomRight.x + point.x;
            result.bottomRight.y = bottomRight.y + point.y;
            return result;
        }
        Rect2D Intersection(const Rect2D &other) const
        {
            Rect2D result;
            int x1 = CategoriseEnd(other.topLeft.x, topLeft.x, bottomRight.x);
            int x2 = CategoriseEnd(other.bottomRight.x, topLeft.x, bottomRight.x);
            int y1 = CategoriseEnd(other.topLeft.y, topLeft.y, bottomRight.y);
            int y2 = CategoriseEnd(other.bottomRight.y, topLeft.y, bottomRight.y);
            if ((x1 == x2) && (x1 != 0)) {
                // TODO: No intersection
                result.topLeft.x = result.bottomRight.x = 0;
            } else {
                result.topLeft.x = (x1 == -1) ? topLeft.x : other.topLeft.x;
                result.bottomRight.x = (x2 == 1) ? bottomRight.x : other.bottomRight.x;
            }
            if ((y1 == y2) && (y1 != 0)) {
                // TODO: No intersection
                result.topLeft.y = result.bottomRight.y = 0;
            } else {
                result.topLeft.y = (y1 == -1) ? topLeft.y : other.topLeft.y;
                result.bottomRight.y = (y2 == 1) ? bottomRight.y : other.bottomRight.y;
            }
            return result;
        }
    private:
        static int CategoriseEnd(Unit end, Unit a, Unit b)
        {
            if (end < a)
                return -1;
            if (end > b)
                return 1;
            return 0;   // It's between a and b
        }
    } Rect2D;
    typedef struct Frame2D {
        Point2D origin, size;
        bool Contains(Point2D point) const
        {
            return (origin.x <= point.x) && (origin.y <= point.y) && ((point.x - origin.x) < size.x) && ((point.y - origin.y) < size.y);
        }
    } Frame2D;
    static inline Frame2D Convert(const Rect2D &rect)
    {
        return (Frame2D){rect.topLeft, {rect.bottomRight.x - rect.topLeft.x, rect.bottomRight.y - rect.topLeft.y}};
    }
    static inline Rect2D Convert(const Frame2D &frame)
    {
        return (Rect2D){frame.origin, {frame.origin.x + frame.size.x, frame.origin.y + frame.size.y}};
    }
    
    class Matrix2D : public Library::Maths::Matrix<Unit, 3>
    {
    public:
        Matrix2D(bool identity = false)
        :Matrix(identity)
        {
        }
        Matrix2D(const Unit values[9])
        :Matrix(values)
        {
        }
        Point2D Apply(const Point2D &point) const
        {
            Unit result[3] = {
                ((*this)(0, 0) * point.x) + ((*this)(0, 1) * point.y) + (*this)(0, 2),
                ((*this)(1, 0) * point.x) + ((*this)(1, 1) * point.y) + (*this)(1, 2),
                ((*this)(2, 0) * point.x) + ((*this)(2, 1) * point.y) + (*this)(2, 2),
            };
            return (Point2D){result[0] / result[2], result[1] / result[2]};
        }
        Rect2D Apply(const Rect2D &area) const
        {
            Graphics::Point2D bounds[] = {
                Apply(area.topLeft),
                Apply(area.bottomRight),
                Apply((Graphics::Point2D){area.topLeft.x, area.bottomRight.y}),
                Apply((Graphics::Point2D){area.bottomRight.x, area.topLeft.y}),
            };
            Graphics::Rect2D result = (Graphics::Rect2D){bounds[0], bounds[0]};
            for (int i = 1; i < 4; i++) {
                if (bounds[i].x < result.topLeft.x)
                    result.topLeft.x = bounds[i].x;
                if (bounds[i].x > result.bottomRight.x)
                    result.bottomRight.x = bounds[i].x;
                if (bounds[i].y < result.topLeft.y)
                    result.topLeft.y = bounds[i].y;
                if (bounds[i].y > result.bottomRight.y)
                    result.bottomRight.y = bounds[i].y;
            }
            return result;
        }
        Matrix2D Invert(void) const
        {
            Matrix2D minors;
            minors(0, 0) = ((*this)(1, 1) * (*this)(2, 2)) - ((*this)(2, 1)) * ((*this)(1, 2));
            minors(0, 1) = ((*this)(1, 0) * (*this)(2, 2)) - ((*this)(2, 0)) * ((*this)(1, 2));
            minors(0, 2) = ((*this)(1, 0) * (*this)(2, 1)) - ((*this)(2, 0)) * ((*this)(1, 1));
            minors(1, 0) = ((*this)(0, 1) * (*this)(2, 2)) - ((*this)(2, 1)) * ((*this)(0, 2));
            minors(1, 1) = ((*this)(0, 0) * (*this)(2, 2)) - ((*this)(2, 0)) * ((*this)(0, 2));
            minors(1, 2) = ((*this)(0, 0) * (*this)(2, 1)) - ((*this)(2, 0)) * ((*this)(0, 1));
            minors(2, 0) = ((*this)(0, 1) * (*this)(1, 2)) - ((*this)(1, 1)) * ((*this)(0, 2));
            minors(2, 1) = ((*this)(0, 0) * (*this)(1, 2)) - ((*this)(1, 0)) * ((*this)(0, 2));
            minors(2, 2) = ((*this)(0, 0) * (*this)(1, 1)) - ((*this)(1, 0)) * ((*this)(0, 1));
            Unit determinant = ((*this)(0, 0) * minors(0, 0)) - ((*this)(0, 1) * minors(0, 1)) + ((*this)(0, 2) * minors(0, 2));
            Matrix2D result;
            result(0, 0) =  minors(0, 0) / determinant;
            result(0, 1) = -minors(1, 0) / determinant;
            result(0, 2) =  minors(2, 0) / determinant;
            result(1, 0) = -minors(0, 1) / determinant;
            result(1, 1) =  minors(1, 1) / determinant;
            result(1, 2) = -minors(2, 1) / determinant;
            result(2, 0) =  minors(0, 2) / determinant;
            result(2, 1) = -minors(1, 2) / determinant;
            result(2, 2) =  minors(2, 2) / determinant;
            return result;
        }
    };
    
    class Colour
    {
    public:
        UInt8 red, green, blue, alpha;
        
        static const Colour White;
        static const Colour Black;
    };
    
    class Path
    {
    public:
        class Entry
        {
        public:
            Entry(Point2D location)
            :_point(location)
            {
            }
            
            Point2D _point;
        };
        
    private:
        Library::Array<Entry> _entries;
        Matrix2D _matrix;
        
        Path(Matrix2D matrix)
        :_matrix(matrix)
        {
        }
        
    public:
        Library::Array<Entry>::ConstIterator begin(void) const
        {
            return _entries.begin();
        }
        Library::Array<Entry>::ConstIterator end(void) const
        {
            return _entries.end();
        }

        Path(Point2D start, Matrix2D matrix = Matrix2D(true))
        :_matrix(matrix)
        {
            _entries.Add(Entry(_matrix.Apply(start)));
        }
        void LineTo(Point2D point)
        {
            _entries.Add(Entry(_matrix.Apply(point)));
        }
        void Add(const Path &otherPath)
        {
            for (const auto& entry : otherPath)
                _entries.Add(Entry(_matrix.Apply(entry._point)));
        }
        static Path Rect(const Rect2D &rect)
        {
            Graphics::Path result(rect.topLeft);
            result.LineTo((Graphics::Point2D){rect.bottomRight.x, rect.topLeft.y});
            result.LineTo(rect.bottomRight);
            result.LineTo((Graphics::Point2D){rect.topLeft.x, rect.bottomRight.y});
            return result;
        }
        Rect2D Bounds(void) const
        {
            Rect2D result;
            
            result.topLeft = result.bottomRight = _entries[0]._point;
            for (const auto& entry : _entries) {
                if (entry._point.x < result.topLeft.x)
                    result.topLeft.x = entry._point.x;
                if (entry._point.y < result.topLeft.y)
                    result.topLeft.y = entry._point.y;
                if (entry._point.x > result.bottomRight.x)
                    result.bottomRight.x = entry._point.x;
                if (entry._point.y > result.bottomRight.y)
                    result.bottomRight.y = entry._point.y;
            }
            return result;
        }
        Path Apply(const Matrix2D &matrix) const
        {
            Path result(matrix);
            for (const auto& entry : _entries)
                result.Add(entry._point);
            return result;
        }
    };
    
    class FrameBuffer
    {
    public:
        typedef enum {
            Format24RGB,
            Format24BGR,
            Format32RGBA,   // Fourth byte is alpha
            Format32RGBx,   // Fourth byte is nothing
            Format32BGRx,   // Fourth byte is nothing
            Format8GA,      // Grayscale, gray + alpha
        } Format;

        virtual ~FrameBuffer() {}
    
        virtual Format Type(void) const = 0;
        virtual UInt32 Width(void) const = 0;
        virtual UInt32 Height(void) const = 0;
        virtual UInt8* Buffer(void) const = 0;
        UInt32 PixelSize(void) const;
    };
    class Bitmap : public FrameBuffer
    {
    public:
        Bitmap();
        Bitmap(UInt32 width, UInt32 height, Format type, const UInt8 *data = NULL);
        Bitmap(const Bitmap &other);
        ~Bitmap();
        
        Bitmap& operator =(const Bitmap &other);
        
        Format Type(void) const;
        UInt32 Width(void) const;
        UInt32 Height(void) const;
        UInt8* Buffer(void) const;
        
    private:
        Format _format;
        UInt32 _width, _height;
        UInt8 *_data;
    };

    /* Internal classes */
    class Clipping
    {
    public:
        Clipping(const Path &path, const Clipping *parent);
        Clipping(const Clipping &other);
        const Rect2D& Bounds(void) const { return _bounds; }
        const Library::Array<Library::Array<int>>& Clippings(void) const { return *_clippings; }
    private:
        Rect2D _bounds;
        Library::SharedPointer<Library::Array<Library::Array<int>>> _clippings;
    };
    class State
    {
    public:
        State(Rect2D frame, const State *parent)
        :matrix(true), clipping(Path::Rect(frame), parent ? &parent->clipping : NULL)
        {
            if (parent)
                matrix = parent->matrix;
        }
        State(const State &other)
        :matrix(other.matrix), clipping(other.clipping)
        {
        }
        
        Matrix2D matrix;
        Clipping clipping;
    };
    
    /* A nice graphics context */
    class Context
    {
    private:
        
        Library::Array<State> _states;
        FrameBuffer &_target;
        const Context *_parent;
        
        State& CurrentState(void)
        {
            return _states[_states.Count() - 1];
        }
        const State& CurrentState(void) const
        {
            return _states[_states.Count() - 1];
        }

    public:
        Context(FrameBuffer &target)
        :_target(target), _parent(NULL)
        {
            _states.Add(State((Rect2D){{0, 0}, {Unit(target.Width()), Unit(target.Height())}}, NULL));
        }
        Context(const Context &parent)
        :_target(parent._target), _parent(&parent)
        {
            _states.Add(State((Rect2D){{0, 0}, {Unit(_target.Width()), Unit(_target.Height())}}, &_parent->CurrentState()));
        }
        
        // Management
        void Push(void)
        {
            _states.Add(CurrentState());
        }
        void Pop(void)
        {
            UInt32 count = _states.Count();
            if (count == 1)
                /* TODO: error */;
            else
                _states.Remove(count - 1);
        }
        
        void SetClipping(const Path &path)
        {
            State &state = CurrentState();
            state.clipping = Clipping(path.Apply(state.matrix), _parent ? &_parent->CurrentState().clipping : NULL);
        }
        
        // Matrix
        void Translate(Unit x, Unit y)
        {
            Unit matrix[] = {
                1, 0, x,
                0, 1, y,
                0, 0, 1
            };
            CurrentState().matrix *= Matrix2D(matrix);
        }
        void Rotate(Unit angle)
        {
            Unit c = Library::Maths::Cosine(angle);
            Unit s = Library::Maths::Sine(angle);
            Unit matrix[] = {
                c,  s,  0,
                -s, c,  0,
                0,  0,  1
            };
            CurrentState().matrix *= Matrix2D(matrix);
        }
        void Scale(Unit x, Unit y)
        {
            Unit matrix[] = {
                x, 0, 0,
                0, y, 0,
                0, 0, 1
            };
            CurrentState().matrix *= Matrix2D(matrix);
        }
        void Apply(Matrix2D transform)
        {
            CurrentState().matrix *= transform;
        }
        
        const Matrix2D& CurrentTransform(void) const
        {
            return CurrentState().matrix;
        }
        
        void DrawPolygon(const Path &path, const Colour &colour);  // Solid fill
        void DrawBitmap(const Path &path, const FrameBuffer &bitmap); // Bitmap fill
        void DrawBitmap(const FrameBuffer &bitmap); // Bitmap fill
        void DrawBitmapTinted(const Path &path, const FrameBuffer &bitmap, const Colour &tint); // Bitmap fill
        void DrawBitmapTinted(const FrameBuffer &bitmap, const Colour &tint); // Bitmap fill
        void DrawLine(const Path &path, const Colour &colour, Unit width);  // Lines only
        // TODO: gradient
    };
}

#endif /* Context_hpp */
