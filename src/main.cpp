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

#include <iostream>
#include <string>

#include "Tracer.h"

int main(int argc, char *argv[]) {
    bool forceHostCpu = false;
    if (argc < 3) {
        std::cout << "Usage: ./raytracer scenefile.txt samples_per_pixel [image_width] [image_height] [forceHostCPU]" << std::endl;
        return 1;
    }
    uint samplesPerPixel = atoi(argv[2]);
    if (argc >= 6) forceHostCpu = true;

    // open scene file
    auto loadedScene = Tracer::SceneFile::Load(argv[1]);
    auto &imageSize = loadedScene.GetImageDimensions();

    // override image size of scene file if image size specified in cmd line args
    if (argc >= 5) {
        imageSize[0] = atoi(argv[3]);
        imageSize[1] = atoi(argv[4]);
    }

    Tracer::Renderer renderer(forceHostCpu);

    std::cout << "Samples Per Pixel: " << samplesPerPixel << std::endl;
    std::cout << "Rendering using " << renderer.GetDeviceName() << std::endl;

    // now render
    Tracer::Image img = renderer.RenderScene(loadedScene.GetScene(), loadedScene.GetCamera(), samplesPerPixel, imageSize[0], imageSize[1]);

    img.WritePNG(loadedScene.GetSceneName() + ".png");
}
