#include "Material.h"

#include <cstdio>
#include <string>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>
#include "Common.h"

using Tracer::MaterialManager;
using Tracer::Material;
using Tracer::Vector3f;
using Tracer::uint;

static std::string testMaterialFileContents =
R""(

newmtl DiffuseMaterial
Ns 96.0                         # ignored
Ka 0.000000 0.000000 0.000000   # ignored
Kd 0.200000 0.900000 0.500000   # diffuse
Ks 0.000000 0.000000 0.000000   # specular
Ke 0.000000 0.000000 0.000000   # emission
Ni 1.000000                     # ignored
d 1.000000                      # ignored
illum 2                         # type of material (diffuse)

newmtl SpecularMaterial
Ns 96.0                         # ignored
Ka 0.000000 0.000000 0.000000   # ignored
Kd 0.000000 0.000000 0.000000   # diffuse
Ks 0.700000 0.600000 0.500000   # specular
Ke 0.000000 0.000000 0.000000   # emission
Ni 1.000000                     # ignored
d 1.000000                      # ignored
illum 3                         # type of material (specular)

newmtl RefractiveMaterial
Ns 96.0                         # ignored
Ka 0.000000 0.000000 0.000000   # ignored
Kd 0.000000 0.000000 0.000000   # diffuse
Ks 0.200000 0.300000 0.400000   # specular
Ke 0.000000 0.000000 0.000000   # emission
Ni 1.000000                     # ignored
d 1.000000                      # ignored
illum 4                         # type of material (refraction)

newmtl LightMaterial
Ns 96.0                         # ignored
Ka 0.000000 0.000000 0.000000   # ignored
Kd 0.000000 0.000000 0.000000   # diffuse
Ks 0.000000 0.000000 0.000000   # specular
Ke 0.900000 0.800000 0.700000   # emission
Ni 1.000000                     # ignored
d 1.000000                      # ignored
illum 2                         # type of material (diffuse)

)"";

///
/// \brief Test opening a material from a file and managing materials
///
class MaterialManagerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        // create tmp material file
        filename = tmpnam(nullptr);
        file.open(filename);
        file << testMaterialFileContents;
        file.close();

        // create empty file
        emptyFilename = tmpnam(nullptr);
        file.open(emptyFilename);
        file << "# this has no materials in it :(\n";
        file.close();
    }
    virtual void TearDown() {
        // delete the tmp test files
        remove(filename.c_str());
        remove(emptyFilename.c_str());
    }
    std::string filename;
    std::string emptyFilename;
    std::ofstream file;
    MaterialManager mm;
};

TEST_F(MaterialManagerTest, Accessors) {
    Material newMaterial = Material(Vector3f(0,0,0),Vector3f(1,1,1),Material::DIFFUSE);
    uint id = mm.AddMaterial(newMaterial);
    // the first material has an id of "0"
    EXPECT_EQ(id, 0);

    // check that stored material matches
    EXPECT_EQ(mm.GetMaterial(id), newMaterial);

    // modify the material
    Material modifiedMaterial = newMaterial;
    modifiedMaterial.materialType = Material::REFRACTION;
    mm.SetMaterial(modifiedMaterial, id);
    // check it matches
    EXPECT_EQ(mm.GetMaterial(id), modifiedMaterial);
    EXPECT_NE(mm.GetMaterial(id), newMaterial);

    const std::vector<Material> &materials = mm.GetMaterials();
    EXPECT_EQ(materials.size(), 1);
    EXPECT_EQ(materials[id], modifiedMaterial);

    uint secondId = mm.AddMaterial(newMaterial);
    EXPECT_EQ(secondId, 1);
    EXPECT_EQ(materials.size(), 2);
    EXPECT_EQ(materials[secondId], newMaterial);
}

TEST_F(MaterialManagerTest, ReadMaterialsFromMtlFile) {
    uint firstId = mm.GetMaterial(filename);
    EXPECT_EQ(firstId, 0);

    // Get each material by name now
    EXPECT_EQ(  mm.GetMaterial(filename, "DiffuseMaterial"),    0);
    EXPECT_EQ(  mm.GetMaterial(filename, "SpecularMaterial"),   1);
    EXPECT_EQ(  mm.GetMaterial(filename, "RefractiveMaterial"), 2);
    EXPECT_EQ(  mm.GetMaterial(filename, "LightMaterial"),      3);

    // check that the materials are correct
    EXPECT_EQ(  mm.GetMaterial(0), Material(Vector3f(0,0,0),Vector3f(.2F,.9F,.5F),Material::DIFFUSE)   );
    EXPECT_EQ(  mm.GetMaterial(1), Material(Vector3f(0,0,0),Vector3f(.7F,.6F,.5F),Material::SPECULAR)  );
    EXPECT_EQ(  mm.GetMaterial(2), Material(Vector3f(0,0,0),Vector3f(.2F,.3F,.4F),Material::REFRACTION));
    EXPECT_EQ(  mm.GetMaterial(3), Material(Vector3f(.9F,.8F,.7F),Vector3f(0,0,0),Material::DIFFUSE)   );
}

TEST_F(MaterialManagerTest, ReadMaterialsFromMtlFileOutOfOrder) {
    // the order in the material file determines id, not order requested
    EXPECT_EQ(  mm.GetMaterial(filename, "LightMaterial"),      3);
    EXPECT_EQ(  mm.GetMaterial(filename, "SpecularMaterial"),   1);
    EXPECT_EQ(  mm.GetMaterial(filename, "RefractiveMaterial"), 2);
    EXPECT_EQ(  mm.GetMaterial(filename, "DiffuseMaterial"),    0);
}

TEST_F(MaterialManagerTest, RequestNonExistentMaterial) {
    EXPECT_THROW( mm.GetMaterial(filename, "BlackHole"), MaterialManager::MaterialNotFoundException );
}

TEST_F(MaterialManagerTest, NoMaterialsInMaterialFile) {
    EXPECT_THROW( mm.GetMaterial(emptyFilename), MaterialManager::MaterialNotFoundException );
}
