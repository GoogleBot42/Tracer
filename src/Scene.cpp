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

#include "Scene.h"

#include <fstream>
#include <sstream>
#include <string>

#include "Common.h"
#include "Material.h"
#include "Camera.h"

namespace Tracer {

SceneFile SceneFile::Load(std::string filename) {
    std::fstream file(filename, std::fstream::in);
    if (!file) throw FileReadException(filename);

    // vars for camera (there can only be one in a scene file and the values are split across lines
    Vector3f eye, look, up;
    float focalLength = 0;
    Vector<float,4> imagePlaneBounds({0,0,0,0});
    Vector<uint,2> imageResolution({0,0});
    // vars to check the user enter all required values
    bool hasEye = false;
    bool hasLook = false;
    bool hasUp = false;
    bool hasFocalLength = false;
    bool hasImagePlaneBounds = false;
    bool hasImageResolution = false;

    Scene scene;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineParser(line);
        std::string type;

        if (!(lineParser >> type)) {
            // do nothing blank line
        } else if (type[0] == '#') { // comment
            // do nothing
        } else if (type == "eye") {
            float x,y,z;
            if (!(lineParser >> x >> y >> z))
                throw ParseException("Could not parse eye details in driver file");
            eye = Vector3f(x,y,z);
            hasEye = true;
        } else if (type == "look") {
            float x,y,z;
            if (!(lineParser >> x >> y >> z))
                throw ParseException("Could not parse look details in driver file");
            look = Vector3f(x,y,z);
            hasLook = true;
        } else if (type == "up") {
            float x,y,z;
            if (!(lineParser >> x >> y >> z))
                throw ParseException("Could not parse up details in driver file");
            up = Vector3f(x,y,z);
            hasUp = true;
        } else if (type == "d") {
            if (!(lineParser >> focalLength))
                throw ParseException("Could not parse d details in driver file");
            hasFocalLength = true;
        } else if (type == "bounds") {
            float w,x,y,z;
            if (!(lineParser >> w >> x >> y >> z))
                throw ParseException("Could not parse camera image bounds details in driver file");
            imagePlaneBounds = Vector<float,4>({w,x,y,z});
            hasImagePlaneBounds = true;
        } else if (type == "res") {
            int x,y;
            if (!(lineParser >> x >> y))
                throw ParseException("Could not parse resolution details in driver file");
            imageResolution = Vector<uint,2>({static_cast<uint>(x),static_cast<uint>(y)});
            hasImageResolution = true;
        } else if (type == "sphere") {
            float x,y,z,r,  emission_r,emission_g,emission_b,   color_r,color_g,color_b;
            int materialType;
            if (!(lineParser >> x >> y >> z >> r >> emission_r >> emission_g >> emission_b >> color_r >> color_g >> color_b >> materialType))
                throw ParseException("Could not parse sphere details in driver file");
            scene.AddPrimative(
                        Sphere(r, Vector3f(x,y,z)),
                        Material(Vector3f(emission_r,emission_g,emission_b), Vector3f(color_r,color_g,color_b), (Material::MaterialType)materialType));
        } else {
            throw ParseException("Unexpected scene item in driver file: \"" + line + "\"");
        }
    }

    file.close();

    if (!(hasEye && hasLook && hasUp && hasFocalLength && hasImagePlaneBounds && hasImageResolution)) {
        // at least one required value was missed... failure
        throw ParseException(R"(Not all required values were present in the driver file. You must include all of: ["eye","look","up","d","bounds","res"].)");
    }

    // parse filename to remove path and extension to get the scene name
    // first remove path
    const uint64 last_slash_idx = filename.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
        filename.erase(0, last_slash_idx + 1);
    // next remove extension
    const uint64 period_idx = filename.rfind('.');
    if (std::string::npos != period_idx)
        filename.erase(period_idx);

    return SceneFile(std::move(scene), Camera(up, look, eye, focalLength, imagePlaneBounds), imageResolution, filename);
}

} // namespace Tracer
