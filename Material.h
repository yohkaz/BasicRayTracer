#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vec3.h"

class Material {
public:
    Material() = default;
    Material(const Vec3<float>& color, float kd): color(color), kd(kd) {}
    Vec3<float> evaluateColorResponse(const Vec3<float>& normal, const Vec3<float>& wi) const {
        float cosAngle = std::max(dot(normal, -wi), 0.f);
        return color*kd*cosAngle;
    }

private:
    Vec3<float> color;
    float kd;     // diffuse coefficient
};

#endif