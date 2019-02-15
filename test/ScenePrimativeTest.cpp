#include "ScenePrimative.h"

#include <gtest/gtest.h>
#include <iostream>
#include "Vector.h"

using Tracer::ScenePrimative;
using Tracer::Intersection;
using Tracer::Sphere;
using Tracer::Vector3f;
using Tracer::Ray;

///
/// \brief Test storage of primatives (there is only one primative right now so very boring...)
///
class ScenePrimativeTest : public ::testing::Test {
protected:
    ScenePrimative p1 = ScenePrimative(Sphere(1, Vector3f(1,2,3)),0);
    ScenePrimative p2 = ScenePrimative(Sphere(2, Vector3f(3,3,3)),1);
};

TEST_F(ScenePrimativeTest, Accessors) {
    EXPECT_EQ(p1.GetMaterialId(), 0);
    EXPECT_EQ(p2.GetMaterialId(), 1);
    // you cannot actually get the sphere inside of the ScenePrimative since there is no need to do so yet
}

TEST_F(ScenePrimativeTest, Intersection) {
    // no matter what primative is stored inside, they have to be able to intersect with a ray
    EXPECT_EQ(p1.Intersect(Ray(Vector3f(0,0,0),Vector3f(1,2,3))), (Intersection { 0.472251F, Vector3f(-0.267261F,-0.534522F,-0.801784F), Vector3f(0.472251F,0.944502F,1.41675F) }) );
    EXPECT_EQ(p2.Intersect(Ray(Vector3f(0,0,0),Vector3f(1,2,-3))), Intersection::NO_INTERSECTION());
}
