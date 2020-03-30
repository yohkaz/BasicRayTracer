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
        auto& q = hemisphereMapping.getValue(wIndex);
        auto w = hemisphereMapping.getDir(wIndex);

        // x = nOrigin, y = nHit
        // Q'(x, w) = (1 - lr) * Q(x, w)
        //            + lr * (Le(y, -w) + [max/integral]fs(wi, w) * cosThetaI * Q(y, wi))
        Vec3<float> qUpdated = (1.f - lr) * q
                               + lr * (irradiance + estimateIntegral(nHit, w, material));

        if (qUpdated.length() < 0.5f)
            qUpdated = normalize(Vec3<float>(1, 1, 1)) * 0.5f;

        if (qUpdated.length() > 10000.f)
            qUpdated = normalize(qUpdated) * 10000.f;

        hemisphereMapping.updateByIndex(wIndex, qUpdated);
    }

private:
    Vec3<float> maxValue(const BVH::Node* y,
                         const Vec3<float>& w,
                         const Material& material) {
        auto ret = table.insert({y, HemisphereMapping(resX, resY)});
        auto& mapping = ret.first->second;
 
        int wiIndex = 0;
        auto Qy = mapping.getValue(wiIndex);
        float maxLengthQy = Qy.length();
        for (int i = 0; i < (int) mapping.size(); i++) {
            auto& currentQy = mapping.getValue(i);
            if (currentQy.length() > maxLengthQy) {
                wiIndex = i;
                Qy = currentQy;
                maxLengthQy = currentQy.length();
            }
        }

        // Need to multiply by cosAngle and fs
        //Vec3<float> wi = mapping.getDir(wiIndex);
        //Vec3<float> normal(0.f, 0.f, 1.f);
        //float cosAngle = std::max(dot(wi, normal), 0.f);
        //Vec3<float> fs = material.evaluateBRDF(normal, wi, w);
        //return fs * Qy * cosAngle;
        return Qy;
    }

    Vec3<float> estimateIntegral(const BVH::Node* y,
                                 const Vec3<float>& w,
                                 const Material& material) {
        auto ret = table.insert({y, HemisphereMapping(resX, resY)});
        auto& mapping = ret.first->second;
 
        Vec3<float> sum(0.f, 0.f, 0.f);
        Vec3<float> normal(0.f, 0.f, 1.f);
        for (int i = 0; i < (int) mapping.size(); i++) {
            auto& Qy = mapping.getValue(i);
            Vec3<float> wi = mapping.getDir(i);
            float cosAngle = std::max(dot(wi, normal), 0.f);
            Vec3<float> fs = material.evaluateBRDF(normal, wi, w);
            sum += fs * Qy * cosAngle;
        }

        return sum * ((2.f * M_PI) / (float) mapping.size());
        //return sum / (float) mapping.size();
    }

    // Node -> hemisphere
    // x in R^3 -> score (probability) for each direction from x
    std::map<const BVH::Node*, HemisphereMapping> table;
    int resX;
    int resY;
    float lr;   // learning rate
};

