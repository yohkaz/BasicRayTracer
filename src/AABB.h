#ifndef AABB_H
#define AABB_H

class AABB {
public:
    AABB() = default;

    AABB(const Vec3<float>& minBound, const Vec3<float>& maxBound):
        minBound(minBound), maxBound(maxBound) {}

    AABB(const std::vector<Vec3<float>>& vertices) {
        compute(vertices);
    }

    void compute(const std::vector<Vec3<float>>& vertices) {
        minBound = vertices[0];
        maxBound = vertices[0];

        for (const auto& v: vertices) {
            minBound[0] = std::min(v[0], minBound[0]);
            minBound[1] = std::min(v[1], minBound[1]);
            minBound[2] = std::min(v[2], minBound[2]);

            maxBound[0] = std::max(v[0], maxBound[0]);
            maxBound[1] = std::max(v[1], maxBound[1]);
            maxBound[2] = std::max(v[2], maxBound[2]);
        }
    }

    const Vec3<float>& getMinBound() const { return minBound; }
    const Vec3<float>& getMaxBound() const { return maxBound; }

private:
    Vec3<float> minBound;
    Vec3<float> maxBound;
};

#endif