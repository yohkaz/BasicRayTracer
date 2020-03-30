#ifndef HEMISPHERE_SAMPLING_H
#define HEMISPHERE_SAMPLING_H

#include "Vec3.h"
#include "Ray.h"

class HemisphereSampling {
public:
    struct Sample {
        Vec3<float> direction;
        // extra info
        int index;
        float probability;
    };

    Vec3<float> uniformSample(float u1, float u2) const {
        float r = std::sqrt(1.0f - u1 * u1);
        float phi = 2 * PI * u2;
 
        return Vec3<float>(std::cos(phi) * r, std::sin(phi) * r, u1);
    }

    virtual void sampleDirection(Sample& s) const {
        //std::cout << "RandomSampling" << std::endl;
        float u1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float u2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        s.direction = uniformSample(u1, u2);
        s.probability = 1.f / (2.f * M_PI);
    }
};

class CosigneWeighted : public HemisphereSampling {
public:
    Vec3<float> cosineWeightedSample(float u1, float u2) const {
        float r = std::sqrt(u1);
        float theta = 2 * PI * u2;

        float x = r * std::cos(theta);
        float y = r * std::sin(theta);

        // direction in tangent space
        return Vec3<float>(x, y, std::sqrt(std::max(0.0f, 1 - u1)));
    }

    void sampleDirection(Sample& s) const override {
        //std::cout << "CosigneWeighted" << std::endl;
        float u1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float u2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        // direction in tangent space
        s.direction = cosineWeightedSample(u1, u2);
        float cosAngle = std::max(dot(Vec3<float>(0.f, 0.f, 1.f), s.direction), 0.f);
        s.probability = cosAngle / M_PI;
    }
};

#endif