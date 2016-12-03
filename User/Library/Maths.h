#ifndef __MATHS_H__
#define __MATHS_H__

#include "Runtime.h"

namespace Library {
    namespace Maths {
        
        const double Pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745;

        template<class Unit> Unit Absolute(const Unit &value)
        {
            return (value < 0) ? -value : value;
        }
        
        template<class Unit> Unit Sine(Unit theta)
        {
            const Unit B = 4 / Pi;
            const Unit C = -4 / (Pi * Pi);
            const Unit P = 0.225;
            
            // fix this
            while (theta > Pi)
                theta -= 2 * Pi;
            while (theta < -Pi)
                theta += 2 * Pi;
            
            Unit y = B * theta + C * theta * Absolute(theta);
            return P * (y * Absolute(y) - y) + y;
        }

        template<class Unit> Unit Cosine(const Unit &theta)
        {
            return Sine(theta + (Pi / 2));
        }
        
        template<class Unit> Unit ATan2(Unit y, Unit x)
        {
            const Unit ONEQTR_PI = Pi / 4.0;
            const Unit THRQTR_PI = 3.0 * Pi / 4.0;
            Unit r, angle;
            Unit abs_y = Absolute(y) + 1e-10f;      // kludge to prevent 0/0 condition
            if (x < 0.0f) {
                r = (x + abs_y) / (abs_y - x);
                angle = THRQTR_PI;
            } else {
                r = (x - abs_y) / (x + abs_y);
                angle = ONEQTR_PI;
            }
            angle += (0.1963f * r * r - 0.9817f) * r;
            if (y < 0.0f)
                return -angle;     // negate if in quad III or IV
            else
                return angle;
        }

        template<class Type, int Edge> class Matrix
        {
        private:
            Type _values[Edge * Edge];
        public:
            Matrix(bool identity = false)
            {
                for (UInt32 i = 0; i < (Edge * Edge); i++)
                    _values[i] = 0;
                if (identity)
                    for (UInt32 i = 0; i < Edge; i++)
                        (*this)(i, i) = 1;
            }
            Matrix(const Type values[Edge * Edge])
            {
                for (UInt32 i = 0; i < (Edge * Edge); i++)
                    _values[i] = values[i];
            }
            Type& operator()(UInt32 row, UInt32 col)
            {
                return _values[(Edge * row) + col];
            }
            Type operator()(UInt32 row, UInt32 col) const
            {
                return _values[(Edge * row) + col];
            }
            Matrix operator*(const Matrix &other) const
            {
                Matrix result;
                for (UInt32 row = 0; row < Edge; row++) {
                    for (UInt32 col = 0; col < Edge; col++) {
                        Type value = 0;
                        for (UInt32 i = 0; i < Edge; i++)
                            value += (*this)(row, i) * other(i, col);
                        result(row, col) = value;
                    }
                }
                return result;
            }
            Matrix& operator*=(const Matrix &other)
            {
                Matrix result = (*this)*other;
                for (UInt32 i = 0; i < (Edge * Edge); i++)
                    _values[i] = result._values[i];
                return *this;
            }
        };
        
    }
}

#endif // __MATHS_H__
