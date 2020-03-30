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
            up = Vec3<float>(0, 1, 1);
            right = normalize(cross(up, n));
            up = normalize(cross(n, right));
        }

    Vec3<float> getPosition() const override {
        // Random numbers between -0.5 and 0.5
        float randomUp = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f;
        float randomRight = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f;

        return position + randomUp*up*size + randomRight*right*size;
    }

    const Vec3<float>& getNormal() const {
        return n;
    }

    bool pointInArea(const Vec3<float>& p) const {
        float proj1 = dot(p, up*size) / (size);
        float proj2 = dot(p, right*size) / (size);

        if((proj1 < size && proj1 > 0) && (proj2 < size && proj2 > 0))
            return true;

        return false;
    }

    Vec3<float> getMinPoint() const {
        Vec3<float> minPoint = position + up*size*(-0.5f) + right*size*(-0.5f);
        //Vec3<float> maxPoint = position + up*size*(0.5f) + right*size*(0.5f);

        // swap if needed
        //for (int i = 0; i < 3; i++) {
        //    if (minPoint[i] > maxPoint[i])
        //        std::swap(minPoint[i], maxPoint[i]);
        //}

        return minPoint;
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