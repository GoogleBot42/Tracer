// Copyright (c) 2019 Matthew J. Runyan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef TRACER_VECTOR_H
#define TRACER_VECTOR_H

#include <initializer_list>
#include <iostream>
#include <type_traits>
#include <SYCL/sycl.hpp>

#include "Common.h"

namespace Tracer {

///
/// \brief A basic, base vector type whose contents is stored in-place (no malloc).
/// This is a fundamental structure for types such as Vector3d,Color,Pixel,etc
///
/// T: type of data that is stored per element of the vector (it is expected that this is a primative)
/// N: the size of the vector
///
/// See Vector3 and Vector3f for specialized versions
///
template<typename T, uint N>
class Vector {
public:
    ///
    /// \brief Initializes the vector using a C++11 initializer list
    /// Conveniently, create a new vector using "Vector<float,6> vec({1,2,3,4,5,6})"
    /// (Sadly, std::initializer_list doesn't allow statically checking initializer list size so I don't want to use it
    ///
    Vector(std::initializer_list<T> list) {
        // no way to statically check the size of a std::initializer_list... :/
        // make sure we don't run over bad memory because std::initializer_list size != N
        auto it = list.begin();
        auto end = list.end();
        for (uint i=0; i<N; i++) {
            if (it == end) break;
            data_[i] = *it;
            ++it;
        }
    }
    ///
    /// \brief Accesses an element of the vector
    ///
    T &operator [](uint64 i) { return data_[i]; }
    ///
    /// \brief Gets an element of the vector
    ///
    const T &operator [](uint64 i) const { return data_[i]; }
    ///
    /// \brief Returns the size of the vector
    ///
    uint Size() const { return N; }
    ///
    /// \brief Calculates and returns the element-wise vector addition
    ///
    Vector<T,N> operator+(const Vector<T,N> &b) const {
        Vector<T,N> result;
        for (uint i=0; i<N; i++)
            result[i] = (*this)[i] + b[i];
        return result;
    }
    ///
    /// \brief Calculates and returns the element-wise vector subtraction
    ///
    Vector<T,N> operator-(const Vector<T,N> &b) const {
        Vector<T,N> result;
        for (uint i=0; i<N; i++)
            result[i] = (*this)[i] - b[i];
        return result;
    }
    ///
    /// \brief Calculates a vector, scalar multiplication
    ///
    Vector<T,N> operator*(float b) const {
        Vector<T,N> result;
        for (uint i=0; i<N; i++)
            result[i] = (*this)[i] * b;
        return result;
    }
    ///
    /// \brief Adds a vector to the currect vector using element-wise vector addition
    ///
    Vector<T,N> &operator+=(const Vector<T,N> &b) {
        for (uint i=0; i<N; i++)
            (*this)[i] += b[i];
        return *this;
    }
    ///
    /// \brief Subtracts a vector from the currect vector using element-wise vector subtraction
    ///
    Vector<T,N> &operator-=(const Vector<T,N> &b) {
        for (uint i=0; i<N; i++)
            (*this)[i] -= b[i];
        return *this;
    }
    ///
    /// \brief Returns true if two vectors are equal
    ///
    bool operator==(const Vector<T,N> &b) const {
        for (uint i=0; i<N; i++)
            if (std::is_same<T,float>::value || std::is_same<T,double>::value) {
                // if floating point type, check if they are roughly equal
                if (!(FLOAT_EQ((*this)[i], b[i])))
                    return false;
            } else {
                if (!((*this)[i] == b[i]))
                    return false;
            }
        return true;
    }
    ///
    /// \brief Returns true if two vectors are not equal
    ///
    bool operator!=(const Vector<T,N> &b) const {
        return !((*this) == b);
    }
    ///
    /// \brief Calculates and returns the element-wise vector multiplication
    ///
    Vector<T,N> Multiply(const Vector<T,N> &b) const {
        Vector<T,N> result;
        for (uint i=0; i<N; i++)
            result[i] = (*this)[i] * b[i];
        return result;
    }
    ///
    /// \brief Calculates and returns the dot product
    ///
    float Dot(const Vector<T,N> &b) const {
        float result = 0;
        for (uint i=0; i<N; i++)
            result += (*this)[i] * b[i];
        return result;
    }
    ///
    /// \brief Calculates and returns the length of the vector
    ///
    float Length() const {
        float result = 0;
        for (uint i=0; i<N; i++)
            result += (*this)[i] * (*this)[i];
        return cl::sycl::sqrt(result);
    }
    void Print() const {
        std::cout << '{';
        for (uint i=0; i<N; i++) {
            std::cout << (*this)[i];
            if (i < N-1) std::cout << ',';
        }
        std::cout << '}' << std::endl;
    }
private:
    Vector() = default;
    T data_[N];
};

template<typename T>
class Vector3 : public Vector<T,3> {
public:
    Vector3(std::initializer_list<T> list) : Vector<T,3>(list) {
    }
    Vector3(T x = 0, T y = 0, T z = 0) : Vector<T,3>({x,y,z}) {
    }
    Vector3(const Vector<T,3> &v) : Vector<T,3>(v) {
    }
    inline T &X() { return (*this)[0]; }
    inline T &Y() { return (*this)[1]; }
    inline T &Z() { return (*this)[2]; }
    inline const T &X() const { return (*this)[0]; }
    inline const T &Y() const { return (*this)[1]; }
    inline const T &Z() const { return (*this)[2]; }
};

class Vector3f : public Vector3<float> {
public:
    Vector3f(std::initializer_list<float> list) : Vector3<float>(list) {
    }
    Vector3f(float x = 0, float y = 0, float z = 0) : Vector3<float>({x,y,z}) {
    }
    Vector3f(const Vector3<float> &v) : Vector3<float>(v) {
    }
    Vector3f(const Vector<float,3> &v) : Vector3<float>(v) {
    }
    ///
    /// \brief Calculates and returns the cross product
    ///
    Vector3f Cross(const Vector3f &b) const {
        // y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x
        return Vector3f(Y()*b.Z()-Z()*b.Y(), Z()*b.X()-X()*b.Z(), X()*b.Y()-Y()*b.X());
    }
    ///
    /// \brief Ensures that the components vector are all clamped to [0,1]
    ///
    Vector3f Clamp() const { return Vector3f(CLAMP_0_1(X()), CLAMP_0_1(Y()), CLAMP_0_1(Z())); }
    ///
    /// \brief Normalizes the vector
    ///
    Vector3f &Normalize() { return *this = *this * (1/Length()); }
};

///
/// \brief A simple 3rd dim Ray
///
struct Ray {
    Vector3f origin, direction;
    Ray(Vector3f origin_, Vector3f direction_) : origin(origin_), direction(direction_) {}
};

} // namespace Tracer

#endif // TRACER_VECTOR_H
