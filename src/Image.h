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

#ifndef TRACER_IMAGE_H
#define TRACER_IMAGE_H

#include <initializer_list>

#include <SYCL/sycl.hpp>
#include "Common.h"
#include "Vector.h"

using cl::sycl::pow;

namespace Tracer {

///
/// \brief Simple representation of a color where each component of the color is [0.0F,1.0F]
/// The bounds of the component values are not continually updated. (See Clamp())
///
class Color : public Vector3f {
public:
    Color(float r=0, float g=0, float b=0) : Color({r,g,b}) {
    }
    Color(std::initializer_list<float> list) : Vector3f(list) {
    }
    Color(const Vector3f &v) : Vector3f(v) {
    }
    inline float &R() { return X(); }
    inline float &G() { return Y(); }
    inline float &B() { return Z(); }
    inline const float &R() const { return X(); }
    inline const float &G() const { return Y(); }
    inline const float &B() const { return Z(); }
};

///
/// \brief Simple representation of a pixel on an image where each component of the pixel is [0,255]
///
class Pixel : public Vector3<uint8> {
public:
    Pixel(uint8 r=0, uint8 g=0, uint8 b=0) : Pixel({r,g,b}) {
    }
    Pixel(std::initializer_list<uint8> list) : Vector3<uint8>(list) {
    }
    Pixel(const Color &c) {
        Color fixed = c.Clamp();
        R() = static_cast<uint8>(fixed.R()*255.0F);
        G() = static_cast<uint8>(fixed.G()*255.0F);
        B() = static_cast<uint8>(fixed.B()*255.0F);
    }
    inline uint8 &R() { return X(); }
    inline uint8 &G() { return Y(); }
    inline uint8 &B() { return Z(); }
    inline const uint8 &R() const { return X(); }
    inline const uint8 &G() const { return Y(); }
    inline const uint8 &B() const { return Z(); }
    ///
    /// \brief Apply gamma correction
    ///
    Pixel &GammaCorrect() {
        auto gamma = [](uint8 &color) {
            // color = 255 * (color/255)^(1/2.2)
            color = static_cast<uint8>((255.0F * pow(static_cast<float>(color)/255.0F, 1/2.2F)) +.5F);
        };
        gamma(R()); gamma(G()); gamma(B());
        return *this;
    }
};

///
/// \brief A simple wrapper around an array for holding an image.
/// \note Assumes only three channels (r,g,b) and 24 bits per pixel
///
class Image {
public:
    Image(uint width, uint height) : width_(width), height_(height) {
        data_ = new Pixel[GetDataSize()];
    }
    // copying is expensive, yo
    Image(const Image& img) = delete;
    // moving is cheap
    Image(Image&& img) {
        data_  = img.data_;
        width_ = img.width_;
        height_= img.height_;
        img.data_  = nullptr;
        img.width_ = 0;
        img.height_= 0;
    }
    ~Image() {
        delete data_;
    }
    ///
    /// \brief Gets the pixel at x,y
    ///
    Pixel &GetPixel(uint x, uint y) { return data_[y*width_+x]; }
    ///
    /// \brief Sets the pixel at x,y
    ///
    void SetPixel(uint x, uint y, Pixel p) { GetPixel(x,y) = p; }
    ///
    /// \brief Returns the raw data for this image (mind that Image isn't destroyed while in use)
    ///
    Pixel *GetData() { return data_; }
    ///
    /// \brief Converts an image to raw data for use in external functions that do not understand Tracer::Image or Tracer::Pixel (memory MUST be freed)
    ///
    uint8 *CreateRawImage() {
        uint8 *newImg = new uint8[3*GetDataSize()];
        for (uint x=0; x<GetWidth(); x++) {
            for (uint y=0; y<GetHeight(); y++) {
                uint offset = 3* ((GetHeight()-y-1)*GetWidth() + x);
                newImg[offset+0] = GetPixel(x,y).R();
                newImg[offset+1] = GetPixel(x,y).G();
                newImg[offset+2] = GetPixel(x,y).B();
            }
        }
        return newImg;
    }
    ///
    /// \brief Returns the total size of the image buffer in bytes
    ///
    uint GetDataSize() { return width_ * height_; }
    ///
    /// \brief Gets the width of the image
    ///
    uint GetWidth() { return width_; }
    ///
    /// \brief Gets the height of the image
    ///
    uint GetHeight() { return height_; }
    ///
    /// \brief Writes the image to a png file
    ///
    void WritePNG(const std::string &filename);
public:
    ///
    /// \brief The raw image data
    ///
    Pixel *data_;
    ///
    /// \brief The width of the image
    ///
    uint width_;
    ///
    /// \brief The height of the image
    ///
    uint height_;
};

}

#endif // TRACER_IMAGE_H
