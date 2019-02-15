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

#ifndef TRACER_SCENEOBJECT_H
#define TRACER_SCENEOBJECT_H

#include <SYCL/sycl.hpp>
#include "Vector.h"
#include "Common.h"

using cl::sycl::isinf;

namespace Tracer {

///
/// \brief Describes an intersection with a scene primative
///
class Intersection {
public:
    Intersection(float distance, Vector3f normal, Vector3f intersectionPosition)
        : distance_(distance), normal_(normal), intersectionPosition_(intersectionPosition) {}
    ///
    /// \brief Represents a failure to intersect with a primative
    ///
    static inline Intersection NO_INTERSECTION() {
        return Intersection(std::numeric_limits<float>::infinity(),Vector3f(),Vector3f());
    }
    ///
    /// \brief Determines if two intersections are identical (within floating point error) Returns two for comparing two of NO_INTERSECTION()
    ///
    bool operator==(const Intersection &b) const {
        if (isinf(distance_) && isinf(b.distance_)) return true; // check for no intersection
        return FLOAT_EQ(distance_, b.distance_) &&
                normal_ == b.normal_ &&
                intersectionPosition_ == b.intersectionPosition_;
    }
    bool operator!=(const Intersection &b) const { return !((*this) == b); }
    ///
    /// \brief Returns true if the intersection on the lhs is better than the rhs (lhs.distance < rhs.distance)
    ///
    bool operator<(const Intersection &b) const {
        return distance_ < b.distance_;
    }
    ///
    /// \brief Get/Set distance
    ///
    float &Distance() { return distance_; }
    ///
    /// \brief Get distance
    ///
    float Distance() const { return distance_; }
    ///
    /// \brief Get/Set normal
    ///
    Vector3f &Normal() { return normal_; }
    ///
    /// \brief Get normal
    ///
    const Vector3f &Normal() const { return normal_; }
    ///
    /// \brief Get/Set point of intersection
    ///
    Vector3f &IntersectionPosition() { return intersectionPosition_; }
    ///
    /// \brief Get point of intersection
    ///
    const Vector3f &IntersectionPosition() const { return intersectionPosition_; }
private:
    ///
    /// \brief how far the ray needs to go to reach the intersection (infinity if no intersection)
    ///
    float distance_;
    ///
    /// \brief the normal at the point of intersection
    ///
    Vector3f normal_;
    ///
    /// \brief point of intersection
    ///
    Vector3f intersectionPosition_;
};

///
/// \brief Describes a sphere primative
///
class Sphere {
public:
    Sphere(float radius, Vector3f position):
        radius_(radius), position_(position) {}
    ///
    /// \brief Determines if the ray intersects the sphere and where that intersection is.
    /// \param ray
    /// \return IntersectionData with distance == INF if there is no intersection
    /// \note Sphere doesn't know what scene primative it is attached to, so Intersection.intersectedPrimative always == nullptr (See ScenePrimative.Intersect)
    ///
    Intersection Intersect(const Ray &ray) const;
    ///
    /// \brief returns the radius of the sphere
    ///
    float GetRadius() { return radius_; }
    ///
    /// \brief gets the position of the sphere
    ///
    Vector3f GetPosition() { return position_; }
private:
    ///
    /// \brief the radius of the sphere
    ///
    float radius_;
    ///
    /// \brief the position of the sphere
    ///
    Vector3f position_;
};

///
/// \brief Describes a primative that can be rendered in the scene
///
class ScenePrimative {
public:
    ///
    /// \brief Creates a new scene object primative from a sphere
    ///
    ScenePrimative(const Sphere &sphere, uint materialId) : sceneObjectType_(SCENE_OBJECT_SPHERE), materialId_(materialId) {
        sceneObjectData_.sphere = sphere;
    }
//    ///
//    /// \brief Creates a new scene object primative from a triangle
//    ///
//    SceneObject(Triangle triangle) : sceneObjectType(SCENE_OBJECT_TRIANGLE) {
//        sceneObjectData.triangle = triangle;
//    }
    ///
    /// \brief determines if the ray intersects the scene object
    /// \return information about the intersection
    ///
    Intersection Intersect(const Ray &ray) const;
    ///
    /// \brief Gets the id of the material associated with this primative
    ///
    uint GetMaterialId() const { return materialId_; }
private:
    ///
    /// \brief For handling different primative types differently (since virtual functions on GPU is impossible)
    ///
    enum SceneObjectType {
        SCENE_OBJECT_SPHERE = 0,
        // SCENE_OBJECT_TRIANGLE // TODO
        SCENE_PRIMATIVES_COUNT
    };
    ///
    /// \brief Used to determine the type of primative stored inside of the scene primative
    ///
    SceneObjectType sceneObjectType_;
    ///
    /// \brief Holds the actual primative in a memory efficient union
    /// \note
    /// This CANNOT use std::variant, this is an C++17 feature which ComputeCpp and/or SYCL 1.2.1 doesn't support
    /// Only C++11 is supported in this version.
    ///
    /// It is acknowledged that using a union can lead to many issues.  The existense of this class seeks
    /// to isolate this problem as much as possible such that the union should be safe.
    ///
    union SceneObjectData {
        SceneObjectData() {}
        // the objects that can be represented
        Sphere sphere;
        // Triangle triangle;
    } sceneObjectData_;
    ///
    /// \brief the id of the material of the primative
    ///
    uint materialId_;
};

}

#endif // TRACER_SCENEOBJECT_H
