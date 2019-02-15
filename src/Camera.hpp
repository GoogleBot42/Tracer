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

#include "Tracer.h"

#include "Vector.h"

///
/// Why is this a '.hpp' and not a '.cpp' file?
/// Any code that is run in a kernel in SYCL must appear in the same file.
/// By including this '.hpp' file it allows for the SYCL kernel to compile
/// at the cost of increased compile time in the single file where the
/// SYCL kernel is defined.
///
/// See Renderer.cpp for kernel definition.
///
/// The alternative would be to have all SYCL kernel code in headers
/// which is much worse.
///

namespace Tracer {

Ray Camera::GenerateLookForPixel(uint pixelX, uint pixelY, uint imageWidth, uint imageHeight) const {
    // calculate camera vectors w,v,u (the localized unit vectors of the camera)

    // w always points away from the lookat point... for some historical reason
    const Vector3f w = Vector3f(eye_ - look_).Normalize();
    const Vector3f u = up_.Cross(w).Normalize();
    // the up vector is not necessisarily orthogonal to w
    const Vector3f v = w.Cross(u).Normalize(); // normalize just in case i guess

    const float left = imagePlaneBounds_[0];
    const float bottom = imagePlaneBounds_[1];
    const float right = imagePlaneBounds_[2];
    const float top = imagePlaneBounds_[3];

    // where this pixel is on the image plane
    const float px = (static_cast<float>(pixelX))/(imageWidth-1) * (right-left) + left;
    const float py = (static_cast<float>(pixelY))/(imageHeight-1) * (top-bottom) + bottom;
    // put into real world coords where the pixel is at using the camera's localized unit vectors
    const Vector3f position = eye_ - (w * focalLength_) + (u * px) + (v * py);
    // direction is easy now
    const Vector3f direction = Vector3f(position - eye_).Normalize();

    return Ray(position, direction);
}

}
