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
    typedef struct {
        Point2D topLeft, bottomRight;
        Point2D Size(void)
        {
            return (Point2D){bottomRight.x - topLeft.x, bottomRight.y - topLeft.y};
        }
    } Rect2D;
    
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
        Library::Array<Entry>::ConstIterator Start(void) const
        {
            return _entries.Start();
        }
        Library::Array<Entry>::ConstIterator End(void) const
        {
            return _entries.End();
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
            Library::ForEach(otherPath._entries, [=](const Entry &entry){
                _entries.Add(Entry(_matrix.Apply(entry._point)));
                return true;
            });
            
        }
        Rect2D Bounds(void) const
        {
            Rect2D result;
            
            result.topLeft = result.bottomRight = _entries[0]._point;
            Library::ForEach(_entries, [&](const Entry &entry){
                if (entry._point.x < result.topLeft.x)
                    result.topLeft.x = entry._point.x;
                if (entry._point.y < result.topLeft.y)
                    result.topLeft.y = entry._point.y;
                if (entry._point.x > result.bottomRight.x)
                    result.bottomRight.x = entry._point.x;
                if (entry._point.y > result.bottomRight.y)
                    result.bottomRight.y = entry._point.y;
                return true;
            });
            return result;
        }
        Path Apply(const Matrix2D &matrix) const
        {
            Path result(matrix);
            Library::ForEach(_entries, [&](const Entry &entry){
                result.Add(entry._point);
                return true;
            });
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
        Bitmap(UInt32 width, UInt32 height, Format type, const UInt8 *data = NULL);
        Bitmap(const Bitmap &other);
        ~Bitmap();
        
        Format Type(void) const;
        UInt32 Width(void) const;
        UInt32 Height(void) const;
        UInt8* Buffer(void) const;
        
    private:
        Format _format;
        UInt32 _width, _height;
        UInt8 *_data;
    };
    
    class Context
    {
    private:
        class State
        {
        public:
            State()
            :matrix(true)
            {
            }
            State(const State &other)
            :matrix(other.matrix)
            {
            }
            
            Matrix2D matrix;
        };
        
        Library::Array<State> _states;
        FrameBuffer &_target;
        
    public:
        Context(FrameBuffer &target)
        :_target(target)
        {
            _states.Add(State());
        }
        
        // Management
        void Push(void)
        {
            _states.Insert(0, _states[0]);
        }
        void Pop(void)
        {
            _states.Remove(0);
            if (_states.Count() == 0)
                /* TODO: error */;
        }
        
        // Matrix
        void Translate(Unit x, Unit y)
        {
            Unit matrix[] = {
                1, 0, x,
                0, 1, y,
                0, 0, 1
            };
            _states[0].matrix *= Matrix2D(matrix);
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
            _states[0].matrix *= Matrix2D(matrix);
        }
        void Scale(Unit x, Unit y)
        {
            Unit matrix[] = {
                x, 0, 0,
                0, y, 0,
                0, 0, 1
            };
            _states[0].matrix *= Matrix2D(matrix);
        }
        
        void DrawPolygon(const Path &path, const Colour &colour);  // Solid fill
        void DrawBitmap(const Path &path, const FrameBuffer &bitmap); // Bitmap fill
        void DrawBitmap(const FrameBuffer &bitmap); // Bitmap fill
        void DrawLine(const Path &path, const Colour &colour, Unit width);  // Lines only
        // TODO: gradient
    };
}

#endif /* Context_hpp */
