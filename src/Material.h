#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vec3.h"
#include "Worley.h"

class Material {
public:
    Material() = default;
    Material(const Vec3<float>& color,
             float kd,
             float alpha = 0.2f, // need to be != 0 and > 0!
             float metallicness = 0.5f,
             float emitted = 0.f): color(color),
                                   emitted(emitted),
                                   kd(kd), alpha(alpha),
                                   metallicness(metallicness), noise(nullptr) {}

    Vec3<float> getColor() const { return color; }

    float getEmittedLevel() const { return emitted; }

    Vec3<float> evaluateColorResponse(const Vec3<float>& normal, const Vec3<float>& wi) const {
        float cosAngle = std::max(dot(normal, wi), 0.f);
        return color*kd*cosAngle;
    }

    Vec3<float> evaluateColorResponse(const Vec3<float>& normal, const Vec3<float>& wi, const Vec3<float>& wo) const {
        float cosAngle = std::max(dot(normal, wi), 0.f);
        return cosAngle*color*evaluateBRDF(normal, wi, wo);
    }

    Vec3<float> evaluateBRDF(const Vec3<float>& normal, const Vec3<float>& wi, const Vec3<float>& wo) const {
        Vec3<float> wh = normalize(wi + wo);
        float NdotH = std::max(dot(normal, wh), 0.00001f);
        float NdotI = std::max(dot(normal, wi), 0.00001f); // to avoid (nan)s, it's canceled by cosAngle so its ok
        float NdotO = std::max(dot(normal, wo), 0.00001f); // to avoid (nan)s, it's canceled by cosAngle so its ok
        float IdotH = dot(wi, wh);

        // GGX Distribution
        Vec3<float> f0 = metallicness*Vec3<float>(0.91f, 0.92f, 0.92f);
        float a2 = alpha*alpha;
        float denominatorGGX = (1.f + (a2 - 1.f)*(NdotH*NdotH));
        float dGGX = a2 / (PI * denominatorGGX * denominatorGGX);

        // Shlick Fresnel Approximation
        Vec3<float> fGGX = f0 + (Vec3<float>(1.f, 1.f, 1.f) - f0)*std::pow((1.f - std::max(0.f, IdotH)), 5);

        // Shlick Geometric Approximation
        float k = alpha * std::sqrt(2.f / PI);
        float geometricWi = NdotI / (k + (1.f-k)*NdotI);
        float geometricWo = NdotO / (k + (1.f-k)*NdotO);
        float gGGX = geometricWo*geometricWi;

        // Specular BRDF
        Vec3<float> fs = (dGGX*fGGX*gGGX) / (4 * NdotI * NdotO);
        // Diffuse BRDF
        Vec3<float> fd = Vec3<float>(kd, kd, kd) / M_PI;

        return (fs + fd);
    }

    void useWorleyNoise(const Worley* worley) {
        noise = worley;
    }

private:
    Vec3<float> color;
    float emitted;
    float kd;           // diffuse coefficient
    float alpha;        // roughness
    float metallicness;
    const Worley* noise;
};

#endif