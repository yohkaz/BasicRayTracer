#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light {
public:
    PointLight(const Vec3<float>& pos, const Vec3<float>& color, float intensity) : Light(pos, color, intensity) {}
};

#endif