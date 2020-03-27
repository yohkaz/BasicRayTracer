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
        auto& q = table.at(nOrigin).getValue(wIndex);
        auto w = table.at(nOrigin).getDir(wIndex);

        // x = nOrigin, y = nHit
        // Q'(x, w) = (1 - lr) * Q(x, w)
        //            + lr * (Le(y, -w) + [max/integral]fs(wi, w) * cosThetaI * Q(y, wi))
        Vec3<float> qUpdated = (1.f - lr) * q
                               + lr * (irradiance + maxValue(nHit, w, material));

        table.at(nOrigin).updateByIndex(wIndex, qUpdated);
    }

private:
    Vec3<float> maxValue(const BVH::Node* y,
                         const Vec3<float>& w,
                         const Material& material) {
        auto ret = table.insert({y, HemisphereMapping(resX, resY)});
        auto& it = ret.first;
 
        auto wi = it->second.getValue(0);
        float maxLengthWi = wi.length();
        for (int i = 0; i < (int) it->second.size(); i++) {
            auto& currentQ = it->second.getValue(i);
            if (currentQ.length() > maxLengthWi) {
                wi = currentQ;
            }
        }

        // Need to multiply by cosAngle and fs
        Vec3<float> normal(0.f, 0.f, 1.f);
        float cosAngle = std::max(dot(wi, normal), 0.f);
        Vec3<float> fs = material.evaluateBRDF(normal, wi, w);
        return fs * wi * cosAngle;
    }

    void estimateIntegral(const Material& material, const Vec3<float>& dir) {
        // TODO: implement, compare to using maxValue
    }

    // Node -> hemisphere
    // x in R^3 -> score (probability) for each direction from x
    std::map<const BVH::Node*, HemisphereMapping> table;
    int resX;
    int resY;
    float lr;   // learning rate
};

