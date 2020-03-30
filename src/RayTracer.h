#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Image.h"
#include "Scene.h"
#include "Ray.h"
#include "BVH.h"
#include "Qtable.h"

class RayTracer {
public:
    RayTracer() : RayTracer(false, false, false, 4) {}
    RayTracer(bool shadow, bool antiAliasing, bool bvh, int antiAliasingRes)
            : shadow(shadow),
              antialiasing(antiAliasing),
              bvh(bvh),
              pathTracing(false),
              cosineWeighted(false),
              learningLT(false),
              aaRes(antiAliasingRes) {}

    void enableShadow() {
        shadow = true;
    }
    void enableAntiAliasing(int res) {
        antialiasing = true;
        aaRes = res;
    }
    void enableBVH(int minSplit=100) {
        bvh = true;
        bvhMinSplit = minSplit;
    }
    void enablePathTracing(int depth, int spp, bool pure=true) {
        pathTracing = true;
        boundDepth = depth;
        samplesPerPixel = spp;
        purePathTracing = pure;
    }

    void enagleCosineWeighted() {
        cosineWeighted = true;
    }

    void enableLearningLT() {
        learningLT = true;
    }

    void render(Image& img, const Scene& scene) {
        int width = img.getWidth();
        int height = img.getHeight();

        if (bvh)
            pBvh = new BVH(scene.getModels(), bvhMinSplit);

        if (learningLT)
            qtable = new Qtable(10, 20, 0.25f); // resX <= resY
        else if (cosineWeighted)
            pHemisphereSampling = new CosigneWeighted();
        else
            pHemisphereSampling = new HemisphereSampling();

        //#pragma omp parallel for collapse(2)
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
        std::cout << "      Shadow:                     " << (shadow == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Anti-Aliasing:              " << (antialiasing == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      BVH:                        " << (bvh == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Path-Tracing:               " << (pathTracing == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Pure Path-Tracing:          " << (purePathTracing == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Cosine Weighted Sampling:   " << (cosineWeighted == 0 ? "OFF" : "ON") << std::endl;
        std::cout << "      Learning Light Transport:   " << (learningLT == 0 ? "OFF" : "ON") << std::endl;
    }

private:
    bool iterateThroughIndices(const Ray& ray,
                  const std::vector<Model*>& models,
                  Ray::Hit& hit) {
        std::map<int, std::vector<int>> indices;
        if (bvh && !pBvh->intersect(ray, indices))
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
            }
        }

        return foundHit;
    }

    bool iterateThroughNodes(const Ray& ray,
                  const std::vector<Model*>& models,
                  Ray::Hit& hit) {
        // BVH required!
        if (!bvh)
            return false;

        std::vector<BVH::Node*> nodesIntersected;
        if (!pBvh->intersect(ray, nodesIntersected))
            return false;

        float e = -1;
        bool foundHit = false;
        Ray::Hit currentHit;
        for (BVH::Node* node: nodesIntersected) {
            for (auto& item: node->indices) {
                Model* model = models[item.first];
                if(ray.intersect(*model, currentHit, item.second) && (currentHit.distance < e || !foundHit)) {
                    hit = currentHit;
                    hit.info = node;
                    foundHit = true;
                    e = hit.distance;
                }
            }
        }

        return foundHit;
    }

    bool rayTrace(const Ray& ray,
                  const std::vector<Model*>& models,
                  Ray::Hit& hit) {
        if (learningLT)
            return iterateThroughNodes(ray, models, hit);
        else
            return iterateThroughIndices(ray, models, hit);
    }

    Vec3<float> computeHitShading(const Ray& ray, const Ray::Hit hit, const Scene& scene) {
        const Model& model = *hit.m;
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

            if(!shadow || !rayTrace(shadowRay, scene.getModels(), shadowHit)
                    || shadowHit.distance > dist(lightPos, hitPosition)) {
                shading += light->getIntensity()
                           * model.getMaterial().evaluateColorResponse(hit.interpolatedNormal,
                                                                       lightDirection,
                                                                       -ray.getDirection());
            }
        }

        return shading;
    }

    HemisphereSampling::Sample sampleDirection(const Ray::Hit& hit) {
        HemisphereSampling::Sample s;
        if (learningLT)
            qtable->sampleDirection(static_cast<const BVH::Node*>(hit.info), s);
        else
            pHemisphereSampling->sampleDirection(s);

        // Compute coordinate system
        const Model& model = *hit.m;
        const auto& vertices = model.getVertices();
        const auto& indices = model.getIndices();

        Vec3<float> n = -normalize(hit.interpolatedNormal);
        Vec3<float> up = vertices[indices[hit.index][0]] - vertices[indices[hit.index][1]];
        Vec3<float> right = normalize(cross(up, n));
        up = normalize(cross(n, right));

        s.direction = s.direction[0] * right + s.direction[1] * up + s.direction[2] * (-n);
        //s.direction = s.direction[0] * right + s.direction[2] * up + s.direction[1] * (-n);
        return s;
    }

    bool recursivePathTrace(const Ray& ray, const Scene& scene, int depth, Vec3<float>& shading,
                            const BVH::Node* origin = nullptr,
                            const int sampleIndex = -1) {
        if (depth == 0)
            return false;

        Ray::Hit hit;
        if (!rayTrace(ray, scene.getModels(), hit))
            return false; // TODO: default shading here (background) ?

        if (purePathTracing) {
            float emittedLevel = hit.m->getMaterial().getEmittedLevel();
            shading = emittedLevel * hit.m->getMaterial().getColor();
        } else {
            // Direct lighting
            shading = computeHitShading(ray, hit, scene);
        }

        // Update Q-table if learning enabled
        auto nHit = static_cast<const BVH::Node*>(hit.info);
        if (learningLT && origin && nHit && sampleIndex >= 0)
            qtable->update(origin, nHit, sampleIndex, shading, hit.m->getMaterial());

        if (purePathTracing)
            if (hit.m->getMaterial().getEmittedLevel() > 0.99) // Considered a light source
                return true;

        Vec3<float> hitPosition = ray.getOrigin() + hit.distance*ray.getDirection();
        auto sample = sampleDirection(hit);
        Ray newRay = Ray(hitPosition, sample.direction, hit.m, hit.index);

        // Indirect lighting
        Vec3<float> indirectShading;
        recursivePathTrace(newRay, scene, depth-1, indirectShading, nHit, sample.index);
        Vec3<float> response = hit.m->getMaterial().evaluateColorResponse(hit.interpolatedNormal,
                                                                  newRay.getDirection(),
                                                                  -ray.getDirection());
        shading += (response * indirectShading) / sample.probability;
        shading[0] = std::min(shading[0], 1.f);
        shading[1] = std::min(shading[1], 1.f);
        shading[2] = std::min(shading[2], 1.f);

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

        shading /= res*res;
        return pathTraced;
    }

    bool computePixelShading(const Vec3<float>& pixelPosition, const Scene& scene, Vec3<float>& shading) {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));

        Ray::Hit hit;
        if (!rayTrace(ray, scene.getModels(), hit))
            return false;

        shading = computeHitShading(ray, hit, scene);

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

    bool shadow;            // Shadows
    bool antialiasing;      // Anti-aliasing
    bool bvh;               // BVH acceleration
    bool pathTracing;       // Path Tracing
    bool purePathTracing;   // Pure Path Tracing (no direct lighting)
    bool cosineWeighted;    // Cosine Weighted Sampling
    bool learningLT;        // Learning Light Transport

    int aaRes;              // Anti-aliasing resolution

    BVH* pBvh;
    int bvhMinSplit;
    int boundDepth;
    int samplesPerPixel;
    HemisphereSampling* pHemisphereSampling;
    Qtable* qtable;
};

#endif