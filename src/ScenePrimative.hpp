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

#include "ScenePrimative.h"

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

inline Intersection Sphere::Intersect(const Ray &ray) const {
    // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    float t;
    const Vector3f op = position_-ray.origin;
    const float epsilon=1.5e-2F;
    const float b=op.Dot(ray.direction);
    const float det_squared=b*b-op.Dot(op)+radius_*radius_;
    if (det_squared<0) return Intersection::NO_INTERSECTION(); // ray missed
    const float det = cl::sycl::sqrt(det_squared);
    // try both possible solutions and pick the one that is closer and in front of the ray
    if (((t=b-det)>epsilon || (t=b+det)>epsilon) == false)
        return Intersection::NO_INTERSECTION(); // ray missed
    // ray hit calculate relavent values
    Vector3f intersection=ray.origin+ray.direction*t; // ray intersection point
    Vector3f normal=Vector3f(intersection-position_).Normalize(); // normal at intersection
    return Intersection(t, normal, intersection);
}

inline Intersection ScenePrimative::Intersect(const Ray &ray) const {
    // forgeting to add a new primative to Intersect could make your life suck, now it cannot happen
    static_assert (ScenePrimative::SCENE_PRIMATIVES_COUNT -1 == ScenePrimative::SCENE_OBJECT_SPHERE, "You must add the new scene primative type to ScenePrimative::Intersect.");

    if (sceneObjectType_ == SCENE_OBJECT_SPHERE)
        return sceneObjectData_.sphere.Intersect(ray);
    // else if (sceneObjectType == SCENE_OBJECT_TRIANGLE)
    //     return sceneObjectData.triangle.Intersect(ray);

    // this line should never be reached... (see above assertion)
    // and no execptions on GPU unfortunately...
    return Intersection::NO_INTERSECTION();
}

} // namespace Tracer
