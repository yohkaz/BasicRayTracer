#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Image.h"
#include "Scene.h"
#include "Ray.h"
#include "BVH.h"

class RayTracer {
public:
    RayTracer() : RayTracer(false, false, false, 4) {}
    RayTracer(bool shadow, bool antiAliasing, bool bvh, int antiAliasingRes)
            : shadow(shadow), antialiasing(antiAliasing), bvh(bvh), aaRes(antiAliasingRes) {}

    void enableShadow() { shadow = true; }
    void enableAntiAliasing(int res) {
        antialiasing = true;
        aaRes = res;
    }
    void enableBVH() { bvh = true; }

    void render(Image& img, const Scene& scene) {
        int width = img.getWidth();
        int height = img.getHeight();

        if (bvh)
            pBvh = new BVH(scene.getModels());

        #pragma omp parallel for collapse(2)
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(i / (float) width, j / (float) height);
                Vec3<float> shading;

                if (antialiasing && !antiAliasing(i, j, img, scene, shading))
                    continue;
                else if (!antialiasing && !computePixelShading(pixelPosition, scene, shading))
                    continue;

                img(i, j) = shading;
            }
        }
    }

    void printInfos() {
        std::cout << "RayTracer.h" << std::endl;
        std::cout << "      Shadow:         " << (shadow == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Anti-Aliasing:  " << (antialiasing == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      BVH:            " << (bvh == 0 ? "OFF" : "ON") << std::endl;
    }

private:
    bool rayTrace(const Ray& ray,
                  const std::vector<Model*>& models,
                  Model* modelToIgnore,
                  Ray::Hit& hit, Model** modelHit) {
        std::map<int, std::vector<int>> indices;
        if (bvh && !pBvh->intersect(ray, hit, indices))
            return false;

        float e = -1;
        bool foundHit = false;
        Ray::Hit currentHit;

        auto it = indices.begin();
        for (std::size_t i = 0; i < models.size() + indices.size(); i++) {
            std::vector<int> relevantIndices;
            if (bvh) {
                if (it == indices.end())
                    break;
                i = it->first;
                relevantIndices = it->second;
                it++;
            }

            Model* model = models[i];
            if(modelToIgnore != model && ray.intersect(*model, currentHit, relevantIndices) && (currentHit.distance < e || !foundHit)) {
                hit = currentHit;
                foundHit = true;
                e = hit.distance;
                if (modelHit)
                    *modelHit = model;
            }
        }

        return foundHit;
    }

    bool computePixelShading(const Vec3<float>& pixelPosition, const Scene& scene, Vec3<float>& shading) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        const std::vector<Light*>& lights = scene.getLights();
        Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));

        Ray::Hit hit;
        Model* p_modelHit = nullptr;
        if (!rayTrace(ray, scene.getModels(), nullptr, hit, &p_modelHit))
            return false;

        shading = Vec3<float>(0.f, 0.f, 0.f);
        for (const auto& light: lights) {
            Vec3<float> hitPosition = ray.getOrigin() + hit.distance*ray.getDirection();
            Vec3<float> lightPos = light->getPosition();
            Vec3<float> lightDirection = normalize(lightPos - hitPosition);

            Ray shadowRay(hitPosition, lightDirection);
            Ray::Hit shadowHit;

            if(!shadow || !rayTrace(shadowRay, scene.getModels(), p_modelHit, shadowHit, nullptr)
                    || shadowHit.distance > dist(lightPos, hitPosition)) {
                // shading += modelHit.getMaterial().evaluateColorResponse(hit.interpolatedNormal, lightDirection*light->getIntensity());
                shading += p_modelHit->getMaterial().evaluateColorResponse(hitPosition,
                                                                        hit.interpolatedNormal,
                                                                        lightDirection,
                                                                        normalize(cameraPosition - hitPosition));
            }
        }

        return true;
    }

    bool antiAliasing(int i, int j, const Image& img, const Scene& scene, Vec3<float>& shading) {
        bool result = false;
        int counter = 0;
        shading = Vec3<float>(0.f, 0.f, 0.f);

        #pragma omp parallel for collapse(2)
        // for (int ki = -(aaRes/2); ki < aaRes/2; ki++) {
        //     for (int kj = -(aaRes/2); kj < aaRes/2; kj++) {
        for (int ki = 0; ki < aaRes; ki++) {
            for (int kj = 0; kj < aaRes; kj++) {
                Vec3<float> currentShading;
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(((i*aaRes)+ki) / (float) (img.getWidth()*aaRes), ((j*aaRes)+kj) / (float) (img.getHeight()*aaRes));
                if (computePixelShading(pixelPosition, scene, currentShading))
                    result = true;
                else
                    currentShading = img(i, j);
                shading += currentShading;
                counter++;
            }
        }

        shading /= counter;
        return result;
    }

    bool shadow;        // Shadows
    bool antialiasing;  // Anti-aliasing
    bool bvh;           // BVH acceleration

    int aaRes;          // Anti-aliasing resolution
    BVH* pBvh;
};

#endif