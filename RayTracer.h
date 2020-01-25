#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Image.h"
#include "Scene.h"
#include "Ray.h"

class RayTracer {
public:
    static void render(Image& img, Scene& scene) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        const std::vector<Light>& lights = scene.getLights();
        scene.getCamera().printInfos();
        int width = img.getWidth();
        int height = img.getHeight();

        std::cout << "Render.h" << std::endl;
        // std::cout << "      .          ." << std::endl;
        // std::cout << "      ";
        #pragma omp parallel for collapse(2)
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(i / (float) width, j / (float) height);
                Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));

                if (i == width-1 && j == height-1)
                    std::cout << "      LastPixelPosition:  " << pixelPosition << std::endl;

                Ray::Hit hit;
                int indexModel;
                if(!rayTrace(ray, scene.getModels(), -1, hit, indexModel))
                    continue;

                const Model& modelHit = scene.getModels()[indexModel];
                Vec3<float> shading(0.f, 0.f, 0.f);
                for (const auto& light: lights) {
                    Vec3<float> hitPosition = ray.getOrigin() + hit.distance*ray.getDirection();
                    Vec3<float> lightDirection = normalize(light.getPosition() - hitPosition);

                    Ray shadowRay(hitPosition, lightDirection);
                    Ray::Hit shadowHit;

                    if(!rayTrace(shadowRay, scene.getModels(), indexModel, shadowHit, indexModel)
                            || shadowHit.distance > dist(light.getPosition(), hitPosition))
                        shading += modelHit.getMaterial().evaluateColorResponse(hit.interpolatedNormal, lightDirection);
                }
                img(i, j) = shading;
            }
        }
    }

private:
    static bool rayTrace(const Ray& ray,
                         const std::vector<Model>& models,
                         int modelToIgnore,
                         Ray::Hit& hit, int& indexModel) {
        float e = -1;
        bool foundHit = false;

        int i = 0;
        Ray::Hit currentHit;
        for(const auto& model: models) {
            if(modelToIgnore != i && ray.intersect(model, currentHit) && (currentHit.distance < e || !foundHit)) {
                hit = currentHit;
                foundHit = true;
                e = hit.distance;
                indexModel = i;
            }
            i++;
        }
        return foundHit;
    }
};

#endif