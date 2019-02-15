#include "Scene.h"

#include <vector>

#include <gtest/gtest.h>

#include "ScenePrimative.h"
#include "Material.h"
#include "Vector.h"

using Tracer::Scene;
using Tracer::ScenePrimative;
using Tracer::Sphere;
using Tracer::Vector3f;
using Tracer::Material;
using Tracer::MaterialManager;

class SceneTest : public ::testing::Test {
protected:
    SceneTest() {
        s.AddPrimative(Sphere(2,Vector3f(1,1,1)), Material());
        s.AddPrimative(Sphere(2,Vector3f(2,2,2)), 0);
    }
    Scene s;
};

TEST_F(SceneTest, Accessors) {
    const std::vector<ScenePrimative> &primatives = s.GetPrimatives();
    EXPECT_EQ(primatives.size(), 2);

    const MaterialManager &mm = s.GetMaterialManager();
    const std::vector<Material> &materials = mm.GetMaterials();
    EXPECT_EQ(materials.size(), 1);
}
