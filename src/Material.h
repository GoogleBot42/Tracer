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

#ifndef SCENE_MATERIAL_H
#define SCENE_MATERIAL_H

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

#include "Vector.h"
#include <Image.h>

namespace Tracer {

struct Material {
    ///
    /// \brief Represents the type of material
    ///
    enum MaterialType {
        // Matte surface (like paper)
        DIFFUSE = 0,
        // Reflective surface (like a mirror)
        SPECULAR = 1,
        // Translucent + Reflective Surface (like glass)
        REFRACTION = 2
    };
    ///
    /// \brief the light that is emitted by this material (for example: a light has this property >0)
    ///
    Color emission;
    ///
    /// \brief color of the material
    /// \todo remove/modify color in favor of diffuse and specular to allow for hybrid diffuse and specular
    ///
    Color color;
    MaterialType materialType;

    // TODO: MATERIAL VALUES TO BE IMPLEMENTED
    // Vec diffuse;
    // Vec specular;
    // double phong;

    Material() : materialType(Material::DIFFUSE) {}
    Material(Color emission, Color color, MaterialType materialType)
        : emission(emission), color(color), materialType(materialType) {}
    bool operator==(const Material &b) const {
        return materialType == b.materialType && emission == b.emission && color == b.color;
    }
    bool operator!=(const Material &b) const { return !((*this) == b); }
};

class MaterialManager {
public:
    MaterialManager() = default;
    // the material manager simply shouldn't be copied as it can create confusion about where materials are going
    MaterialManager(const MaterialManager&) = delete;
    ///
    /// \brief Looks up the index of the material, creates material from file if needed
    /// \param file the file the material is located in
    /// \param materialName the name of the material in the file (omit to select first material defined in file)
    /// \exception throws exceptions: FileReadException, ParseException, and MaterialNotFoundException
    /// \return index of the material
    ///
    uint GetMaterial(const std::string &file, const std::string &materialName = "");
    ///
    /// \brief Get Material from its material index
    ///
    Material& GetMaterial(uint materialIndex) { return materialList_[materialIndex]; }
    ///
    /// \brief Overwrites the material at the material index
    ///
    void SetMaterial(const Material &material, uint materialIndex) { materialList_[materialIndex] = material; }
    ///
    /// \brief Adds Material to the list of materials and returns the material index.
    /// \param file should be unique
    /// \param materialName should be unique
    ///
    uint AddMaterial(const Material &material, std::string file, std::string materialName);
    ///
    /// \brief Adds Material to the list of materials and returns the material index
    ///
    uint AddMaterial(const Material &material);
    ///
    /// \brief Gets readonly view of all materials. Intended for index lookup on the GPU
    ///
    const std::vector<Material> &GetMaterials() const { return materialList_; }

    ///
    /// \brief Represents an error that material requested could not be found
    ///
    class MaterialNotFoundException : public std::runtime_error {
    public:
        MaterialNotFoundException(const std::string &msg) : std::runtime_error(msg) {}
    };
private:
    ///
    /// \brief loads the material file
    /// \exception throws exceptions: FileReadException, ParseException, and MaterialNotFoundException
    /// \return index of the first material of the file
    ///
    uint LoadMaterialFile(const std::string &file);
    ///
    /// \brief Helper function for generating a unique name for a material from the filename and materialName
    ///
    std::string GetUniqueMaterialName(std::string fileName, std::string materialName) { return fileName + ":" + materialName; }
    ///
    /// \brief map of material indexes by filename and name of material
    ///
    std::unordered_map<std::string,uint> materialMap_;
    ///
    /// \brief unordered list of all materials
    ///
    std::vector<Material> materialList_;
};

} // namespace Tracer

#endif // SCENE_MATERIAL_H
