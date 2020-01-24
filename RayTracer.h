#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Image.h"
#include "Scene.h"
#include "Ray.h"

class RayTracer {
public:
    static void render(Image& img, Scene& scene) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        scene.getCamera().printInfos();
        int width = img.getWidth();
        int height = img.getHeight();

        // #pragma omp parallel for
        #pragma omp parallel for collapse(2)
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(i / (float) width, j / (float) height);
                Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));

                if (i == width-1 && j == height-1)
                    std::cout << "      LastPixelPosition:  " << pixelPosition << std::endl;

                rayTrace(ray, pixelPosition, img(i, j), scene.getModels(), scene.getLights());
            }
        }
    }

private:
    static void rayTrace(const Ray& ray,
                         const Vec3<float>& pixelPosition,
                         Vec3<float>& pixelShading,
                         const std::vector<Model>& models,
                         const std::vector<Light>& lights) {
        float e = -1;
        bool foundHit = false;

        for(const auto& model: models) {
            Ray::Hit hit;
            if(ray.intersect(model, hit) && (hit.distance < e || !foundHit)) {
                foundHit = true;
                e = hit.distance;
                Vec3<float> shading(0.f, 0.f, 0.f);
                for (const auto& light: lights)
                    shading += model.getMaterial().evaluateColorResponse(hit.normal, normalize(pixelPosition - light.getPosition()));
                pixelShading = shading;
            }
        }
    }
};

#endif