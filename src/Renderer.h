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

#ifndef TRACER_RENDERER_H
#define TRACER_RENDERER_H

#include <string>

#include <SYCL/sycl.hpp>
#include "Scene.h"
#include "Image.h"
#include "Vector.h"
#include "Camera.h"
#include "ScenePrimative.h"

namespace Tracer {

///
/// \brief Renderers render scenes
///
class Renderer {
public:
    ///
    /// \brief Constructs a Renderer for rendering a scene
    /// \param forceHostCpu if true, the host cpu will be used (powered by OpenMP) otherwise the fastest device is used
    ///
    Renderer(bool forceHostCpu = false);
    ///
    /// \brief Returns the device used for rendering
    ///
    cl::sycl::device GetDevice() { return queue_.get_device(); }
    ///
    /// \brief Returns the name of the device that is being used to render
    ///
    std::string GetDeviceName() { return GetDevice().get_info<cl::sycl::info::device::name>(); }
    ///
    /// \brief Renders a given scene and returns the image result (renders using the scene's primary camera)
    ///
    Image RenderScene(const Scene &scene, const Camera &camera, uint samplesPerPixel, uint width, uint height);
    ///
    /// \brief Renders a scene using a pre-existing image as the result
    /// \param additive if true, the pre-existing pixels of the image are added to rather than overwritten
    ///
    void RenderScene(const Scene &scene, const Camera &camera, uint samplesPerPixel, Image *image);
    ///
    /// \brief Contains the random seed passed around while rendering a pixel for a seed.
    /// A random seed is essential for rendering a scene accurately.
    /// \todo Generate random seed(s) on the host and pass to the device
    ///
    struct RenderRandomSeed {
        uint s1,s2;
    };
private:
    ///
    /// \brief Samples, once, the color of the scene in some direction (intended to be run on the SYCL device)
    ///
    static Color SampleLight(Ray r, const ScenePrimative *primatives, uint64 primativesCount, const Material *materials, uint64 materialsCount, RenderRandomSeed *seed);
    ///
    /// \brief The SYCL work queue
    ///
    cl::sycl::queue queue_;
};

}

#endif // TRACER_RENDERER_H
