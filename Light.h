#ifndef LIGHT_H
#define LIGHT_H

#include "Vec3.h"

class Light {
public:
    Light(const Vec3<float>& pos, const Vec3<float>& col, float intensity) :
        position(pos), color(col), intensity(intensity) {}

    const Vec3<float>& getPosition() const { return position; }
    const Vec3<float>& getColor() const { return color; }
    const float& getIntensity() const { return intensity; }

private:
    Vec3<float> position;
    Vec3<float> color;
    float intensity;
};

#endif