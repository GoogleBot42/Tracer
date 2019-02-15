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

#include "Renderer.h"

#include <iostream>
#include <vector>

#include <SYCL/sycl.hpp>
#include "ScenePrimative.hpp"
#include "Camera.hpp"
#include "Material.h"

using cl::sycl::pow;
using cl::sycl::sqrt;
using cl::sycl::fabs;
using cl::sycl::cos;
using cl::sycl::sin;

namespace Tracer {

///
/// \brief Local Rendering helpers
///
namespace {

///
/// \brief The default handler used then there is an asynchronous OR synchronous sycl exception
///
void DefaultErrorHandler(cl::sycl::exception const& e) {
    std::cout << "Caught SYCL exception:\n"
              << e.what() << std::endl;
}

///
/// \brief Returns the closest intersection of the ray for the primatives given.  Or NO_INTERSECTION if no intersection is found.
/// \param primativeId holds the id of the primative that was intersected with (if there was an intersection)
///
Intersection ClosestIntersection(const Ray &r, const ScenePrimative *primatives, uint64 primativesCount, uint64 *primativeId) {
    Intersection bestIntersection = Intersection::NO_INTERSECTION();
    *primativeId = 0;
    for (uint64 i=0; i<primativesCount; i++) {
        Intersection newIntersection = primatives[i].Intersect(r);
        if (newIntersection < bestIntersection) {
            bestIntersection = newIntersection;
            *primativeId = i;
        }
    }
    return bestIntersection;
}

///
/// \brief A simple pseudorandom floating point number generator based on a two byte seed.  The more "random" this is, the better
///
float GetRandom(Renderer::RenderRandomSeed *seed) {
    seed->s1 = 36969 * ((seed->s1) & 65535) + ((seed->s1) >> 16);  // hash the seeds using bitwise AND and bitshifts
    seed->s2 = 18000 * ((seed->s2) & 65535) + ((seed->s2) >> 16);

    unsigned int ires = ((seed->s1) << 16) + (seed->s2);

    // Convert to float
    union {
        float f;
        unsigned int ui;
    } res;

    res.ui = (ires & 0x007fffff) | 0x40000000;  // bitwise AND, bitwise OR

    return (res.f - 2.f) / 2.f;
}

} // namespace

Renderer::Renderer(bool forceHostCpu) {
    auto redirectAsyncExceptionToErrorHander = [] (cl::sycl::exception_list exceptions) {
        for (std::exception_ptr const& e : exceptions) {
            try {
                std::rethrow_exception(e);
            } catch(cl::sycl::exception const& e) {
                DefaultErrorHandler(e);
            }
        }
    };

    // pick device
    cl::sycl::device_selector *selector =
            forceHostCpu ?
                static_cast<cl::sycl::device_selector*>(new cl::sycl::host_selector())
              : static_cast<cl::sycl::device_selector*>(new cl::sycl::default_selector());

    queue_ = cl::sycl::queue(*selector, redirectAsyncExceptionToErrorHander);

    delete selector;
}

Image Renderer::RenderScene(const Scene &scene, const Camera &camera, uint samplesPerPixel, uint width, uint height) {
    Image img(width, height);
    RenderScene(scene, camera, samplesPerPixel, &img);
    return img;
}

void Renderer::RenderScene(const Scene &scene, const Camera &camera, uint samplesPerPixel, Image *image) {
    const std::vector<ScenePrimative> &primativesVector = scene.GetPrimatives();
    const std::vector<Material> &materialsVector = scene.GetMaterialManager().GetMaterials();

    // Get raw arrays. SYCL needs them to transfer to the SYCL device
    const ScenePrimative *primatives = primativesVector.data();
    const Material *materials = materialsVector.data();
    Pixel *pixels = image->GetData();

    // Get sizes of each array so SYCL knows how big the arrays are
    const uint64 primativesCount = primativesVector.size();
    const uint64 materialsCount = materialsVector.size();
    const uint pixelWidth = image->GetWidth();
    const uint pixelHeight = image->GetHeight();
    const uint pixelCount = pixelWidth * pixelHeight;

    // this is where the magic starts
    // begin invoking the SYCL kernel
    try {
        // setup SYCL buffers for transfering the arrays to/from the SYCL device
        // NOTE: scalars, unlike arrays "Just work" with no explicit copying needed
        cl::sycl::buffer<ScenePrimative,1> primativeBuffer(primatives, cl::sycl::range<1>(primativesCount));
        cl::sycl::buffer<Material,1> materialBuffer(materials, cl::sycl::range<1>(materialsCount));
        cl::sycl::buffer<Pixel,1> pixelBuffer(pixels, cl::sycl::range<1>(pixelCount));
        cl::sycl::buffer<Camera,1> cameraBuffer(&camera, cl::sycl::range<1>(1));

        // submit a new job to run on the SYCL device
        queue_.submit([&](cl::sycl::handler& cgh) {
            // accessors make sure that the data is synced on the SYCL device when it's running (where appropriate)
            // when the accessor is destructed, the buffers are automatically synced back to the host (where appropriate)
            auto primativeAccessor = primativeBuffer.get_access<cl::sycl::access::mode::read,cl::sycl::access::target::constant_buffer>(cgh);
            auto materialAccessor = materialBuffer.get_access<cl::sycl::access::mode::read,cl::sycl::access::target::constant_buffer>(cgh);
            auto pixelAccessor = pixelBuffer.get_access<cl::sycl::access::mode::discard_write,cl::sycl::access::target::global_buffer>(cgh);
            auto cameraAccessor = cameraBuffer.get_access<cl::sycl::access::mode::read,cl::sycl::access::target::constant_buffer>(cgh);
            // start parallel workgroups and workitems
            // pixelCount total threads divided into workgroups of size 64
            // TODO: choose optimal workgroup size based on device capabilities instead of hardcoded to 64
            cgh.parallel_for<Tracer::Renderer>(cl::sycl::nd_range<1>(pixelCount, 64), [=](cl::sycl::nd_item<1> item) {
                // Note: We are now actually running on the SYCL device.

                // determine what pixel we are calculating in this thread
                // threadId is unique and is [0,pixelCount]
                uint threadId = static_cast<uint>(item.get_global_id(0));
                // uint x = threadId % pixelWidth;
                uint x = threadId % pixelWidth;
                uint y = threadId / pixelWidth;

                // create random seed (TODO: get a real random number from the host)
                RenderRandomSeed seed = {x,y};

                // now actually render the pixel this thread is supposed to render
                const Camera &cam = cameraAccessor[0]; // the only camera
                Ray ray = cam.GenerateLookForPixel(x, y, pixelWidth, pixelHeight);
                // collect the requested number of samples for this pixel
                Color accumulatedColor(0,0,0);
                for (uint i=0; i<samplesPerPixel; i++)
                    accumulatedColor += SampleLight(ray, primativeAccessor.get_pointer(), primativesCount, materialAccessor.get_pointer(), materialsCount, &seed) * (1.F/samplesPerPixel);

                // write the color to the pixel
                Pixel *p = pixelAccessor.get_pointer();
                p[(y) * pixelWidth + (x)] = Pixel(accumulatedColor).GammaCorrect();
            });
        });

        // wait for the SYCL device to finish
        queue_.wait_and_throw();
    } catch (cl::sycl::exception const& e) {
        DefaultErrorHandler(e);
    }
}

Color Renderer::SampleLight(Ray r, const ScenePrimative *primatives, uint64 primativesCount, const Material *materials, uint64 materialsCount, RenderRandomSeed *seed)
{
    uint depth=0;
    Color accumulatedColor(0,0,0);
    Color accumulatedReflectance(1,1,1);

    while (1) {
        uint64 primativeId = 0;
        // try to intersect
        Intersection intersection = ClosestIntersection(r, primatives, primativesCount, &primativeId);
        // if miss, we're done
        if (intersection == Intersection::NO_INTERSECTION())
            return accumulatedColor;
        // only go so deep
        if (++depth>7) return accumulatedColor;

        // the hit object
        const ScenePrimative &primative = primatives[primativeId];

        // lookup the material
        Material material = materials[primative.GetMaterialId()];

        Vector3f fixedNormal=intersection.Normal().Dot(r.direction)<0?intersection.Normal():intersection.Normal()*-1; // normal facing correct direction
        Color BDRF=material.color; // object color for BRDF modulator

        // accumulate color and reflectance
        accumulatedColor += accumulatedReflectance.Multiply(material.emission);

        // TODO: get round russian roulette based ray bounce termination working (currently hangs OpenCL) I believe the RNG is the problem
        // after depth of 5, stop the light traversal at random based on the surface reflectivity
        /*float p = std::max({BDRF.x, BDRF.y, BDRF.z}); // find largest reflective value
        if (++depth>5)
        {
            if (XorRandom(seed)<p)
                BDRF=BDRF*(1/p);
            else
                return accumulatedColor;
        }*/

        accumulatedReflectance = Color(accumulatedReflectance.Multiply(BDRF));

        // calculate the light based on the material type
        if (material.materialType == Material::DIFFUSE) // Ideal DIFFUSE reflection
        {
            float r1=2*static_cast<float>(M_PI)*GetRandom(seed); // random angle
            float r2=GetRandom(seed), r2s=sqrt(r2); // random distance from center
            Vector3f w = fixedNormal; // normal
            Vector3f u = ( ((fabs(w.X())>.1F) ? Vector3f(0,1) : Vector3f(1) ).Cross(w)).Normalize(); // u is perpendicular to w
            Vector3f v = w.Cross(u); // v is perpendicular to u and w
            Vector3f d = Vector3f(u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).Normalize(); // d is a random reflection ray
            r = Ray(intersection.IntersectionPosition(),d);
            continue;
        }
        else if (material.materialType == Material::SPECULAR) // Ideal SPECULAR reflection
        {
            r = Ray(intersection.IntersectionPosition(),r.direction-intersection.Normal()*2*intersection.Normal().Dot(r.direction));
            continue;
        }
        else // Ideal dielectric Material::REFRACTION
        {
            Ray reflRay(intersection.IntersectionPosition(), r.direction-intersection.Normal()*2*intersection.Normal().Dot(r.direction));
            // Ray from outside going in?
            bool into = intersection.Normal().Dot(fixedNormal) > 0;
            float nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.direction.Dot(fixedNormal), cos2t;
            if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0) {    // Total internal reflection
                r = reflRay;
                continue;
            }
            Vector3f tdir = Vector3f(r.direction*nnt - intersection.Normal()*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).Normalize();
            float a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.Dot(intersection.Normal()));
            float Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25F+.5F*Re,RP=Re/P,TP=Tr/(1-P);
            if (GetRandom(seed)<P){
              accumulatedReflectance = Color(accumulatedReflectance*RP);
              r = reflRay;
            } else {
              accumulatedReflectance = Color(accumulatedReflectance*TP);
              r = Ray(intersection.IntersectionPosition(),tdir);
            }
            continue;
        }
    }
}

}
