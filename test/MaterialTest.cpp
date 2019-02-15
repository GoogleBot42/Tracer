#include "Material.h"

#include <gtest/gtest.h>
#include "Vector.h"

using Tracer::Material;
using Tracer::Vector3f;

// This test more serves to be a core template of a test in gtest in my mind...

class MaterialTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        // m1 is default constructed
        m2 = Material(Vector3f(1.1F,2.2F,3.3F), Vector3f(4.4F,5.5F,6.6F), Material::REFRACTION);
    }
    virtual void TearDown() {
    }
    Material m1;
    Material m2;
};

TEST_F(MaterialTest, Constructor) {
    EXPECT_EQ(m1.emission, Vector3f());
    EXPECT_EQ(m1.color, Vector3f());
    EXPECT_EQ(m1.materialType, Material::DIFFUSE);

    EXPECT_EQ(m2.emission, Vector3f(1.1F,2.2F,3.3F));
    EXPECT_EQ(m2.color, Vector3f(4.4F,5.5F,6.6F));
    EXPECT_EQ(m2.materialType, Material::REFRACTION);
}
