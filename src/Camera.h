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

#ifndef TRACER_CAMERA_H
#define TRACER_CAMERA_H

#include "Vector.h"

namespace Tracer {

///
/// \brief Represents a camera inside of the scene. The camera is used to compute what rays need to be ray traced for each pixel in the output image.
///
class Camera {
public:
    Camera(const Vector3f &up, const Vector3f &look, const Vector3f &eye, float focalLength, const Vector<float,4> &imagePlaneBounds)
        : up_(up), look_(look), eye_(eye), focalLength_(focalLength), imagePlaneBounds_(imagePlaneBounds) {}
    ///
    /// \brief Generates the ray that needs to be rendered for a specific pixel on an image
    ///
    Ray GenerateLookForPixel(uint pixelX, uint pixelY, uint imageWidth, uint imageHeight) const;
private:
    Camera() = default;
    ///
    /// \brief Defines what is up for the camera. (Is the camera upside down?)
    ///
    Vector3f up_;
    ///
    /// \brief Where the camera is looking.  The look minus this determines where the camera is looking (except for the rotation of the camera)
    ///
    Vector3f look_;
    ///
    /// \brief Defines the eye of the camera.  The direction of rays coming out of the camera to the scene is based on this.
    ///
    Vector3f eye_;
    ///
    /// \brief How far the image plane is from the eye of the camera (changing this makes the camera "zoom")
    ///
    float focalLength_;
    ///
    /// \brief The bounds of the image plane
    /// This describes the size and shape of the image plane relative to where the camera is looking
    ///
    /// The following is the distances from the look of the camera the image plane is in each direction
    ///     left = imagePlaneBounds_[0]
    ///     bottom = imagePlaneBounds_[1]
    ///     right = imagePlaneBounds_[2]
    ///     top = imagePlaneBounds_[3]
    /// (It is possible to have a lopsided image where the image plane is not in the center of where the camera is looking)
    ///
    Vector<float,4> imagePlaneBounds_;
};

}

#endif // TRACER_CAMERA_H
