#ifndef AREALIGHT_H
#define AREALIGHT_H

#include <random>
#include "Light.h"

class AreaLight : public Light {
public:
    AreaLight(const Vec3<float>& pos, const Vec3<float>& color, float intensity, const Vec3<float>& dir, float size)
              : Light(pos, color, intensity), direction(normalize(dir)), size(size) {
            // Compute coordinate system
            n = -direction;
            up = Vec3<float>(0, 1, 0);
            right = normalize(cross(up, n));
            up = normalize(cross(n, right));
        }

    Vec3<float> getPosition() const override {
        // Random numbers between -0.5 and 0.5
        float randomUp = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f;
        float randomRight = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f;

        return position + randomUp*up*size + randomRight*right*size;
    }

private:
    Vec3<float> direction;
    float size;             // Square size

    // Coordinate system
    Vec3<float> up;
    Vec3<float> right;
    Vec3<float> n;              // normal to the area plane
    Vec3<float> topLeftCorner;
};

#endif