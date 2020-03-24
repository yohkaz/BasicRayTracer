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
            : shadow(shadow), antialiasing(antiAliasing), bvh(bvh), pathTracing(false), aaRes(antiAliasingRes) {}

    void enableShadow() {
        shadow = true;
    }
    void enableAntiAliasing(int res) {
        antialiasing = true;
        aaRes = res;
    }
    void enableBVH() {
        bvh = true;
    }
    void enablePathTracing(int depth, int spp) {
        pathTracing = true;
        boundDepth = depth;
        samplesPerPixel = spp;
    }

    void render(Image& img, const Scene& scene) {
        int width = img.getWidth();
        int height = img.getHeight();

        if (bvh)
            pBvh = new BVH(scene.getModels());

        #pragma omp parallel for collapse(2)
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                float x = i / (float) width;
                float y = j / (float) height;
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(x, y);
                Vec3<float> shading;

                bool render = false;
                if (pathTracing)
                    render = pathTrace(i, j, img, scene, shading);
                else if (antialiasing)
                    render = antiAliasing(i, j, img, scene, shading);
                else
                    render = computePixelShading(pixelPosition, scene, shading);

                if (render)
                    img(i, j) = shading;
            }
        }
    }

    void printInfos() {
        std::cout << "RayTracer.h" << std::endl;
        std::cout << "      Shadow:         " << (shadow == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Anti-Aliasing:  " << (antialiasing == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      BVH:            " << (bvh == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Path-Tracing:   " << (pathTracing == 0 ? "OFF" : "ON") << std::endl;
    }

private:
    bool rayTrace(const Ray& ray,
                  const std::vector<Model*>& models,
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
            if(ray.intersect(*model, currentHit, relevantIndices) && (currentHit.distance < e || !foundHit)) {
                hit = currentHit;
                foundHit = true;
                e = hit.distance;
                if (modelHit)
                    *modelHit = model;
            }
        }

        return foundHit;
    }

    Vec3<float> computeHitShading(const Ray& ray, const Ray::Hit hit, const Model& model, const Scene& scene) {
        const auto& vertices = model.getVertices();
        const auto& indices = model.getIndices();
        Vec3<float> hitPosition = hit.b0*vertices[indices[hit.index][0]]
                                + hit.b1*vertices[indices[hit.index][1]]
                                + hit.b2*vertices[indices[hit.index][2]];

        Vec3<float> shading(0.f, 0.f, 0.f);
        for (const auto& light: scene.getLights()) {
            Vec3<float> lightPos = light->getPosition();
            Vec3<float> lightDirection = normalize(lightPos - hitPosition);

            Ray shadowRay(hitPosition, lightDirection, &model, hit.index);
            Ray::Hit shadowHit;

            if(!shadow || !rayTrace(shadowRay, scene.getModels(), shadowHit, nullptr)
                    || shadowHit.distance > dist(lightPos, hitPosition)) {
                // shading += modelHit.getMaterial().evaluateColorResponse(hit.interpolatedNormal, lightDirection*light->getIntensity());
                shading += model.getMaterial().evaluateColorResponse(hitPosition,
                                                                     hit.interpolatedNormal,
                                                                     lightDirection,
                                                                     -ray.getDirection());
            }
        }

        return shading;
    }

    Vec3<float> cosineSampleHemisphere(const Ray::Hit& hit, const Model& model) {
        float u1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float u2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        float r = std::sqrt(u1);
        float theta = 2 * PI * u2;

        float x = r * std::cos(theta);
        float y = r * std::sin(theta);

        // direction in tangent space
        Vec3<float> rayDirection(x, y, std::sqrt(std::max(0.0f, 1 - u1)));

        // Compute coordinate system
        const auto& vertices = model.getVertices();
        const auto& indices = model.getIndices();

        Vec3<float> n = -normalize(hit.interpolatedNormal);
        Vec3<float> up = vertices[indices[hit.index][0]] - vertices[indices[hit.index][1]];
        Vec3<float> right = normalize(cross(up, n));
        up = normalize(cross(n, right));

        return rayDirection[0] * right + rayDirection[1] * up + rayDirection[2] * (-n);
    }

    bool recursivePathTrace(const Ray& ray, const Scene& scene, int depth, Vec3<float>& shading) {
        if (depth == 0)
            return false;

        Ray::Hit hit;
        Model* p_modelHit = nullptr;
        if (!rayTrace(ray, scene.getModels(), hit, &p_modelHit))
            return false; // TODO: default shading here (background) ?
        else if (dot(hit.interpolatedNormal, ray.getDirection()) > 0.f)
            return true;

        // Direct lighting
        shading = computeHitShading(ray, hit, *p_modelHit, scene);

        Vec3<float> hitPosition = ray.getOrigin() + hit.distance*ray.getDirection();
        // Vec3<float> randomDirection = normalize(Vec3<float>(rand(), rand(), rand()));
        Vec3<float> randomDirection = cosineSampleHemisphere(hit, *p_modelHit);
        Ray newRay = Ray(hitPosition, randomDirection, p_modelHit, hit.index);

        // Indirect lighting
        Vec3<float> indirectShading;
        recursivePathTrace(newRay, scene, depth-1, indirectShading);
        float cosAngle = std::max(dot(newRay.getDirection(), hit.interpolatedNormal), 0.f);
        Vec3<float> BRDF = p_modelHit->getMaterial().evaluateBRDF(hit.interpolatedNormal,
                                                                  newRay.getDirection(),
                                                                  -ray.getDirection());
        shading += BRDF * indirectShading * cosAngle;

        return true;
    }

    bool pathTrace(int i, int j, const Image& img, const Scene& scene, Vec3<float>& shading) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        shading = Vec3<float>(0.f, 0.f, 0.f);

        bool pathTraced = false;
        int res = std::sqrt(samplesPerPixel);
        for (int k = 0; k < res; k++) {
            for (int l = 0; l < res; l++) {
                //     // Random numbers between -0.5 and 0.5
                //     float randomShiftX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f;
                //     float randomShiftY = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f;
                float step = (0.5f / (float) res);
                float xShift = -0.5f + step + (2.f * k * step);
                float yShift = -0.5f + step + (2.f * l * step);

                float x = (i + xShift) / (float) img.getWidth();
                float y = (j + yShift) / (float) img.getHeight();
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(x, y);

                Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));
                Vec3<float> currentShading(0.f, 0.f, 0.f);
                if (recursivePathTrace(ray, scene, boundDepth, currentShading))
                    pathTraced = true;
                else
                    currentShading = img(i, j); // add background pixel

                shading += currentShading;
            }
        }

        shading /= samplesPerPixel;
        return pathTraced;
    }

    bool computePixelShading(const Vec3<float>& pixelPosition, const Scene& scene, Vec3<float>& shading) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));

        Ray::Hit hit;
        Model* p_modelHit = nullptr;
        if (!rayTrace(ray, scene.getModels(), hit, &p_modelHit))
            return false;

        shading = computeHitShading(ray, hit, *p_modelHit, scene);

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
    bool pathTracing;   // Path Tracing

    int aaRes;          // Anti-aliasing resolution
    BVH* pBvh;
    int boundDepth;
    int samplesPerPixel;
};

#endif