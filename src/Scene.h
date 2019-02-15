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

#ifndef TRACER_SCENE_H
#define TRACER_SCENE_H

#include <limits>
#include <vector>

#include <CL/sycl.hpp>
#include "Vector.h"
#include "Material.h"
#include "ScenePrimative.h"
#include "Camera.h"

namespace Tracer {

class Scene {
public:
    Scene() = default;
    Scene(Scene &&s) {
        scenePrimatives_ = std::move(s.scenePrimatives_);
        materialManager_ = std::move(s.materialManager_);
    }
    ///
    /// \brief Adds a sphere primative to the scene
    ///
    void AddPrimative(const Sphere &sphere, uint materialId) { scenePrimatives_.push_back(ScenePrimative(sphere, materialId)); }
    ///
    /// \brief Adds a sphere primative to the scene
    ///
    void AddPrimative(const Sphere &sphere, const Material &material) {
        scenePrimatives_.push_back(ScenePrimative(sphere, materialManager_.AddMaterial(material)));
    }
    ///
    /// \brief Gets a list of all the primatives in the scene
    ///
    std::vector<ScenePrimative>& GetPrimatives() { return scenePrimatives_; }
    const std::vector<ScenePrimative>& GetPrimatives() const { return scenePrimatives_; }
    ///
    /// \brief Gets the material manager for this scene
    ///
    MaterialManager& GetMaterialManager() { return materialManager_; }
    const MaterialManager& GetMaterialManager() const { return materialManager_; }
private:
    ///
    /// \brief A list of the primatives of the scene
    ///
    std::vector<ScenePrimative> scenePrimatives_;
    ///
    /// \brief The material manager for this scene
    ///
    MaterialManager materialManager_;
};

///
/// \brief Handles loading a scene, camera, and image dimensions from a scene file.
/// \note This doesn't belong in Tracer::Scene, it loads more than just a simple scene
///
class SceneFile {
public:
    ///
    /// \brief Loads a scene from a scene file
    /// \todo Document the scene file format
    ///
    static SceneFile Load(std::string file);
    Scene &GetScene() { return scene_; }
    Camera &GetCamera() { return camera_; }
    Vector<uint,2> &GetImageDimensions() { return imageDimensions_; }
    std::string &GetSceneName() { return sceneName_; }
private:
    SceneFile(Scene &&scene, const Camera &camera, const Vector<uint,2> &imageDimensions, std::string sceneName)
        : scene_(std::move(scene)), camera_(camera), imageDimensions_(imageDimensions), sceneName_(sceneName) {}
    ///
    /// \brief The scene from the scene file
    ///
    Scene scene_;
    ///
    /// \brief The camera from the scene file
    ///
    Camera camera_;
    ///
    /// \brief The output image from the scene file
    ///
    Vector<uint,2> imageDimensions_;
    ///
    /// \brief The filename of the scene file minus file path and extension
    ///
    std::string sceneName_;
};

} // namespace Tracer

#endif // TRACER_SCENE_H
