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

#include "Material.h"

#include <fstream>
#include <sstream>
#include <string>

#include "Common.h"
#include "Vector.h"
#include "Image.h"

namespace Tracer {

namespace {

struct MaterialFileReadInfo {
    std::string name;
    Material material;

    // for now, only one of these is actually used at a time
    // which one that is chosen depends on what illumination model
    // is chosen "illum <N>"
    // "illum {0,1,2} -> ideal diffuse material [diffuse value]
    // "illum {3}" -> ideal reflection material [specular value]
    // "illum {4}" -> ideal reflection+refraction material [specular value]
    // TODO allow materials to be both specular AND diffuse (non ideal diffuse)
    Color diffuse;
    Color specular;
};

// Reads the material[s] defined in a .mtl file
// http://www.paulbourke.net/dataformats/mtl/
std::vector<MaterialFileReadInfo> ReadMaterialFile(const std::string &filename) {
    auto materials = std::vector<MaterialFileReadInfo>();

    std::fstream file(filename, std::fstream::in);
    if (!file) throw FileReadException(filename);

    uint materialCount = 0; // the number of materials found in the file so far
    MaterialFileReadInfo currentMaterial;

    auto addMaterial = [&]() {
        // TODO: allow a material to be both DIFFUSE and REFLECTIVE/REFRACTIVE
        currentMaterial.material.color = (currentMaterial.material.materialType == Material::DIFFUSE) ?
                    currentMaterial.diffuse
                  : currentMaterial.specular;
        materials.push_back(currentMaterial);
    };

    std::string line;
    int lineCounter = 0;
    while (std::getline(file, line)) {
        std::istringstream lineParser(line);
        std::string type;
        lineCounter++;

        if (!(lineParser >> type)) {
            // do nothing blank line
        } else if (type == "#") { // comment
            // comment.push_back(line);
        } else if (type == "newmtl") {
            std::string materialName;
            if (!(lineParser >> materialName))
                throw ParseException("Could not parse material name in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
            if (materialCount > 0) {
                // we have a material we were just working on, save it since we are done with it now that a new one has been defined
                // first, set the color of the object based on the type of material
                addMaterial();
            }
            currentMaterial.name = materialName;
            // yay we have found a new material
            materialCount++;
        } else if (type == "Ns") { // phong
            // TODO implement phong
        } else if (type == "Ka") { // ambient
            // This is intentionally ignored.
            // Ambient lighting is not ideal for environments with global illumination
        } else if (type == "Kd") { // diffuse
            float r,g,b;
            if (!(lineParser >> r >> g >> b))
                throw ParseException("Could not parse diffuse details in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
            currentMaterial.diffuse = Color(r,g,b);
        } else if (type == "Ks") { // specular
            float r,g,b;
            if (!(lineParser >> r >> g >> b))
                throw ParseException("Could not parse specular details in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
            currentMaterial.specular = Color(r,g,b);
        } else if (type == "Ke") { // emission
            float r,g,b;
            if (!(lineParser >> r >> g >> b))
                throw ParseException("Could not parse emission details in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
            currentMaterial.material.emission = Color(r,g,b);
        } else if (type == "Ni") { // index of refraction
            // TODO implement variable refraction
        } else if (type == "d") { // transparency
            // Intentionally ignored
            // Not a goal, removes photorealism, and increases rendering complexity substantially
        } else if (type == "illum") { // illumination model
            int illumModelNum;
            if (!(lineParser >> illumModelNum))
                throw ParseException("Could not parse illumination model details in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
            switch (illumModelNum) {
            case 0:
            case 1:
            case 2:
                currentMaterial.material.materialType = Material::DIFFUSE;
                break;
            case 3:
                currentMaterial.material.materialType = Material::SPECULAR;
                break;
            case 4:
                currentMaterial.material.materialType = Material::REFRACTION;
                break;
            default:
                throw ParseException("Unsupported illumination model \"" + std::to_string(illumModelNum) + "\" in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
            }
        } else {
            throw ParseException("Unknown material info type \"" + type + "\" in material file \"" + filename + "\" on line: " + std::to_string(lineCounter));
        }
    }

    if (materialCount != materials.size()) {
        // we have a final material to commit
        addMaterial();
    }

    file.close();

    return materials;
}

} // namespace

uint MaterialManager::LoadMaterialFile(const std::string &file) {
    auto newMaterials = ReadMaterialFile(file);

    // make sure there are materials to load
    if (newMaterials.size() == 0)
        throw MaterialNotFoundException("No materials found in \"" + file + "\"");

    // we return the id of the first material added
    uint firstId = static_cast<uint>(materialList_.size());

    // add the materials in order
    for (auto newMaterial : newMaterials)
        AddMaterial(newMaterial.material, file, newMaterial.name);

    return firstId;
}

uint MaterialManager::GetMaterial(const std::string &file, const std::string &materialName) {
    std::string uniqueName = GetUniqueMaterialName(file, materialName);

    // lookup the material
    auto search = materialMap_.find(uniqueName);
    if (search == materialMap_.end()) {
        // not found, trying to load from file
        uint firstId = LoadMaterialFile(file);
        if (materialName.empty())
            // user didn't specify material name, just give them the first material in the file
            return firstId;
        // look for the material... again
        search = materialMap_.find(uniqueName);
        if (search == materialMap_.end()) {
            // failed again... it just doesn't exist
            throw MaterialNotFoundException("Could not find material by name \"" + materialName + "\" in file \"" + file + "\"");
        }
    }
    // success!
    return search->second;
}

uint MaterialManager::AddMaterial(const Material &material, std::string file, std::string materialName) {
    // get new id
    uint newMaterialId = AddMaterial(material);

    // add material to lookup structure
    std::string uniqueName = GetUniqueMaterialName(file, materialName);
    materialMap_[uniqueName] = newMaterialId;

    return newMaterialId;
}

uint MaterialManager::AddMaterial(const Material &material) {
    // get new id
    uint newMaterialId = static_cast<uint>(materialList_.size());
    // add material to list
    materialList_.push_back(material);
    return newMaterialId;
}

} // namespace Tracer
