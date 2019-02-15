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

#ifndef TRACER_COMMON_H
#define TRACER_COMMON_H

#include <stdexcept>
#include <string>
#include <cstdint>

namespace Tracer {

typedef uint8_t uint8;
typedef unsigned int uint;
typedef unsigned long int uint64;

///
/// \brief Returns true if the floating point numbers are about the same
///
template<typename T>
bool FLOAT_EQ(T a, T b)
{
    return (a>b?a-b:b-a) < 0.0001F;
}

///
/// \brief Ensures that the input value is [0,1]
///
template<typename T>
T CLAMP_0_1(T x){ return x<0 ? 0 : x>1 ? 1 : x; }

///
/// \brief A exception that represents a failure to parse a file
///
class ParseException : public std::runtime_error {
public:
    ParseException(const std::string &msg) : std::runtime_error(msg) {}
};

///
/// \brief A exception that represents a failure to open a file for reading
///
class FileReadException : public std::runtime_error {
public:
    FileReadException(const std::string &filename) : std::runtime_error("Failed to open file for reading: " + filename) {}
};

} // namespace Tracer

#endif // TRACER_COMMON_H
