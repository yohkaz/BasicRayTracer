#include <map>
#include "HemisphereSampling.h"
#include "BVH.h"
#include "HemisphereMapping.h"
#include "Material.h"

class Qtable {
public:
    Qtable(int resX, int resY, float lr) : resX(resX), resY(resY), lr(lr) {}

    HemisphereSampling* getHemisphereSampler(const BVH::Node* n) {
        auto ret = table.insert({n, HemisphereMapping(resX, resY)});
        auto& it = ret.first;
        return &it->second;
    }

    void sampleDirection(const BVH::Node* n, HemisphereMapping::Sample& s) {
        auto ret = table.insert({n, HemisphereMapping(resX, resY)});
        auto& it = ret.first;
        it->second.sampleDirection(s);
    }

    void update(const BVH::Node* nOrigin, const BVH::Node* nHit, int wIndex,
                const Vec3<float>& irradiance, const Material& material) {
        auto& hemisphereMapping = table.at(nOrigin);
        auto q = hemisphereMapping.getValue(wIndex);
        auto w = hemisphereMapping.getDir(wIndex);

        // x = nOrigin, y = nHit
        // Q'(x, w) = (1 - lr) * Q(x, w)
        //            + lr * (Le(y, -w) + [max/integral]fs(wi, w) * cosThetaI * Q(y, wi))
        float qUpdated = (1.f - lr) * q
                               + lr * (irradiance.length() + approxIntegral(nHit, w, material));

        hemisphereMapping.updateByIndex(wIndex, qUpdated);
    }

private:
    float maxValue(const BVH::Node* y,
                         const Vec3<float>& w,
                         const Material& material) {
        auto ret = table.insert({y, HemisphereMapping(resX, resY)});
        auto& mapping = ret.first->second;
 
        auto Qy = mapping.getValue(0);
        for (int i = 0; i < (int) mapping.size(); i++) {
            auto currentQy = mapping.getValue(i);
            if (currentQy > Qy) {
                Qy = currentQy;
            }
        }

        return Qy;
    }

    float approxIntegral(const BVH::Node* y,
                                 const Vec3<float>& w,
                                 const Material& material) {
        auto ret = table.insert({y, HemisphereMapping(resX, resY)});
        auto& mapping = ret.first->second;
 
        float sum = 0.f;
        Vec3<float> normal(0.f, 0.f, 1.f);
        for (int i = 0; i < (int) mapping.size(); i++) {
            auto Qy = mapping.getValue(i);
            Vec3<float> wi = mapping.getDir(i);
            float cosAngle = std::max(dot(wi, normal), 0.f);
            Vec3<float> fs = material.evaluateBRDF(normal, wi, w);
            sum += fs.length() * Qy * cosAngle;
        }

        //return sum * ((2.f * M_PI) / (float) mapping.size());
        return sum / (float) mapping.size();
    }

    // Node -> hemisphere
    // x in R^3 -> score (probability) for each direction from x
    std::map<const BVH::Node*, HemisphereMapping> table;
    int resX;
    int resY;
    float lr;   // learning rate
};

