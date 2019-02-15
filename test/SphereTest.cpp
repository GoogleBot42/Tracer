#include "ScenePrimative.h"

#include <gtest/gtest.h>
#include <iostream>
#include "Vector.h"

using Tracer::Intersection;
using Tracer::Sphere;
using Tracer::Vector3f;
using Tracer::Ray;

///
/// \brief Test storage of primatives (there is only one primative right now so very boring...)
///
class SphereTest : public ::testing::Test {
protected:
    Sphere s1 = Sphere(1, Vector3f(1,2,3));
    Sphere s2 = Sphere(2, Vector3f(3,3,3));
};

TEST_F(SphereTest, Accessors) {
    EXPECT_EQ(s1.GetRadius(), 1);
    EXPECT_EQ(s2.GetRadius(), 2);
    EXPECT_EQ(s1.GetPosition(), Vector3f(1,2,3));
    EXPECT_EQ(s2.GetPosition(), Vector3f(3,3,3));
}

TEST_F(SphereTest, Intersection) {
    // TODO: more and better intersection tests
    // no matter what primative is stored inside, they have to be able to intersect with a ray
    EXPECT_EQ(s1.Intersect(Ray(Vector3f(0,0,0),Vector3f(1,2,3))), (Intersection { 0.472251F, Vector3f(-0.267261F,-0.534522F,-0.801784F), Vector3f(0.472251F,0.944502F,1.41675F) }) );
    EXPECT_EQ(s2.Intersect(Ray(Vector3f(0,0,0),Vector3f(1,2,-3))), Intersection::NO_INTERSECTION());
}
