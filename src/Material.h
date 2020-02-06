#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vec3.h"
#include "Worley.h"

class Material {
public:
    Material() = default;
    Material(const Vec3<float>& color,
             float kd,
             float alpha = 0.f,
             float metallicness = 0.5f): color(color), kd(kd), alpha(alpha), metallicness(metallicness), noise(nullptr) {}
    Vec3<float> evaluateColorResponse(const Vec3<float>& normal, const Vec3<float>& wi) const {
        float cosAngle = std::max(dot(normal, wi), 0.f);
        return color*kd*cosAngle;
    }

    Vec3<float> evaluateColorResponse(const Vec3<float>& p, const Vec3<float>& normal, const Vec3<float>& wi, const Vec3<float>& wo) const {
        float cosAngle = std::max(dot(normal, wi), 0.f);
        Vec3<float> wh = normalize(wi + wo);
        float NdotH = dot(normal, wh);
        float NdotI = dot(normal, wi);
        float NdotO = dot(normal, wo);
        float IdotH = dot(wi, wh);

        float noiseValue = noise ? noise->eval(p) : 1.f;

        // GGX Distribution
        Vec3<float> f0 = metallicness*Vec3<float>(0.91f, 0.92f, 0.92f);
        float a2 = alpha*alpha;
        if (noise) {
            a2 *= noiseValue*noiseValue;
            f0 *= noiseValue;
        }
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
        Vec3<float> fd = color*kd;

        return cosAngle*(fs + fd);
    }

    void useWorleyNoise(const Worley* worley) {
        noise = worley;
    }

private:
    Vec3<float> color;
    float kd;           // diffuse coefficient
    float alpha;        // roughness
    float metallicness;
    const Worley* noise;
};

#endif