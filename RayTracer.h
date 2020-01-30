#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Image.h"
#include "Scene.h"
#include "Ray.h"

class RayTracer {
public:
    RayTracer() : RayTracer(true, true, 4) {}
    RayTracer(bool shadow, bool antiAliasing, int antiAliasingRes)
            : enableShadow(shadow), enableAA(antiAliasing), aaRes(antiAliasingRes) {
        printInfos();
    }

    void render(Image& img, const Scene& scene) {
        int width = img.getWidth();
        int height = img.getHeight();

        // std::cout << "      .          ." << std::endl;
        // std::cout << "      ";
        #pragma omp parallel for collapse(2)
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(i / (float) width, j / (float) height);
                Vec3<float> shading;

                if (enableAA && !antiAliasing(i, j, img, scene, shading))
                    continue;
                else if (!enableAA && !computePixelShading(pixelPosition, scene, shading))
                    continue;

                img(i, j) = shading;
            }
        }
    }

private:
    bool rayTrace(const Ray& ray,
                         const std::vector<Model*>& models,
                         int modelToIgnore,
                         Ray::Hit& hit, int& indexModel) {
        float e = -1;
        bool foundHit = false;

        int i = 0;
        Ray::Hit currentHit;
        for(const auto& model: models) {
            if(modelToIgnore != i && ray.intersect(*model, currentHit) && (currentHit.distance < e || !foundHit)) {
                hit = currentHit;
                foundHit = true;
                e = hit.distance;
                indexModel = i;
            }
            i++;
        }
        return foundHit;
    }

    bool computePixelShading(const Vec3<float>& pixelPosition, const Scene& scene, Vec3<float>& shading) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        const std::vector<Light*>& lights = scene.getLights();
        Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));

        Ray::Hit hit;
        int indexModel;
        if(!rayTrace(ray, scene.getModels(), -1, hit, indexModel))
            return false;

        const Model& modelHit = *scene.getModels()[indexModel];
        shading = Vec3<float>(0.f, 0.f, 0.f);
        for (const auto& light: lights) {
            Vec3<float> hitPosition = ray.getOrigin() + hit.distance*ray.getDirection();
            Vec3<float> lightDirection = normalize(light->getPosition() - hitPosition);

            Ray shadowRay(hitPosition, lightDirection);
            Ray::Hit shadowHit;

            if(!enableShadow || !rayTrace(shadowRay, scene.getModels(), indexModel, shadowHit, indexModel)
                    || shadowHit.distance > dist(light->getPosition(), hitPosition))
                shading += modelHit.getMaterial().evaluateColorResponse(hit.interpolatedNormal, lightDirection*light->getIntensity());
        }

        return true;
    }

    bool antiAliasing(int i, int j, const Image& img, const Scene& scene, Vec3<float>& shading) {
        bool result = false;
        int counter = 0;
        shading = Vec3<float>(0.f, 0.f, 0.f);

        #pragma omp parallel for collapse(2)
        for (int ki = -(aaRes/2); ki < aaRes/2; ki++) {
            for (int kj = -(aaRes/2); kj < aaRes/2; kj++) {
                Vec3<float> currentShading;
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(((i*aaRes)+ki) / (float) (img.getWidth()*aaRes), ((j*aaRes)+kj) / (float) (img.getHeight()*aaRes));
                if (computePixelShading(pixelPosition, scene, currentShading))
                    result = true;
                else
                    continue;
                shading += currentShading;
                counter++;
            }
        }

        shading /= counter;
        return result;
    }

    void printInfos() {
        std::cout << "RayTracer.h" << std::endl;
        std::cout << "      Shadow:         " << (enableShadow == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Anti-Aliasing:  " << (enableAA == 0 ? "OFF" : "ON") << std::endl;
    }

    bool enableShadow;
    bool enableAA;      // Anti-aliasing
    int aaRes;          // Anti-aliasing resolution
};

#endif