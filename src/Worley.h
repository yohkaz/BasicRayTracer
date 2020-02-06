#ifndef WORLEY_H
#define WORLEY_H

#include <vector>
#include "Vec3.h"

class Worley {
public:
    Worley(int n, float resX, float resY, float resZ) {
        generate(n, resX, resY, resZ);
    };

    float eval(const Vec3<float>& p) const {
        if (featurePoints.empty())
            return 0;

        float minDistance = dist(p, featurePoints[0]);
        for (const auto& point: featurePoints)
            minDistance = std::min(minDistance, dist(p, point));

        return minDistance;
    }

private:
    void generate(int n, float resX, float resY, float resZ) {
        for (int i = 0; i < n; i++) {
            float x = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.5f)*resX;
            float y = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.5f)*resY;
            float z = ((static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.5f)*resZ;
            featurePoints.push_back(Vec3<float>(x, y, z));
        }
    }

    std::vector<Vec3<float>> featurePoints;
};

#endif