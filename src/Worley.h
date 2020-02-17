#ifndef WORLEY_H
#define WORLEY_H

#include <vector>
#include "Vec3.h"

class Worley {
public:
    Worley(int n, float resX, float resY, float resZ) : resX(resX), resY(resY), resZ(resZ) {
        generate(n);
    };

    float eval(const Vec3<float>& p) const {
        if (featurePoints.empty())
            return 0;

        // Vec3<float> pNormalized = normalize(p);
        Vec3<float> pDecimal(p[0] - (int) p[0], p[1] - (int) p[1], p[2] - (int) p[2]);

        float minDistance = dist(pDecimal, featurePoints[0]);
        for (const auto& point: featurePoints)
            minDistance = std::min(minDistance, dist(pDecimal, point));

        // return minDistance / Vec3<float>(resX, resY, resZ).normalize();
        return minDistance / 2;
    }

private:
    void generate(int n) {
        for (int i = 0; i < n; i++) {
            float x = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.5f)*2.f;
            float y = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.5f)*2.f;
            float z = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.5f)*2.f;
            featurePoints.push_back(Vec3<float>(x, y, z));
        }
    }

    std::vector<Vec3<float>> featurePoints;
    float resX;
    float resY;
    float resZ;
};

#endif