#ifndef RAY_H
#define RAY_H

#include <algorithm>
#include "Vec3.h"
#include "Model.h"

class Ray {
public:
    class Hit {
        public:
        Hit() : index(-1), distance(0), b0(0), b1(0), b2(0), info(nullptr) {}
        int index;                      // index of the corresponding triangle
        float distance;                 // distance of the hit
        Vec3<float> faceNormal;         // face normal of the corresponding triangle
        Vec3<float> interpolatedNormal; // interpolated normal at that point

        // barycentric coordinates
        float b0;
        float b1;
        float b2;

        // extra info
        void* info;
    };

    Ray(const Vec3<float>& origin, const Vec3<float>& direction,
        const Model* m = nullptr, int index = -1): origin(origin),
                                                   direction(direction),
                                                   epsilon(0.00001f),
                                                   originModel(m),
                                                   originTriangleIndex(index) {}

    const Vec3<float>& getOrigin() const { return origin; }
    const Vec3<float>& getDirection() const { return direction; }

    bool intersectTriangle(const Vec3f &p0,
                            const Vec3f &p1,
                            const Vec3f &p2,
                            const Vec3<float>& n,
                            Hit& hit) const {
        Vec3f edge1 = p1 - p0, edge2 = p2 - p0;
        Vec3f pvec = cross(direction, edge2);
        float det = dot(edge1, pvec);
        if (fabs (det) < epsilon)
            return false;
        float inv_det = 1.0f / det;
        Vec3f tvec = origin - p0;
        hit.b1 = dot(tvec, pvec) * inv_det;
        Vec3f qvec = cross(tvec, edge1);
        hit.b2 = dot(direction, qvec) * inv_det;
        hit.distance = dot(edge2, qvec) * inv_det;
        hit.b0 = 1.f - hit.b1 - hit.b2;
        if (hit.b1 < 0.f || hit.b1 > 1.f || hit.distance < 0.f)
            return false;
        if (hit.b2 >= 0.f && hit.b1 + hit.b2 <= 1.f)
            return true;
        return false;
    }

    bool intersectAABB(const AABB& aabb) const {
        Vec3<float> tMin = (aabb.getMinBound() - origin) / direction;
        Vec3<float> tMax = (aabb.getMaxBound() - origin) / direction;

        // swap if needed
        for (int i = 0; i < 3; i++) {
            if (tMin[i] > tMax[i])
                std::swap(tMin[i], tMax[i]);
        }

        float tFirstPoint = (tMin[0] > tMin[1]) ? tMin[0] : tMin[1];
        float tSecondPoint = (tMax[0] < tMax[1]) ? tMax[0] : tMax[1];

        if (tFirstPoint > tMax[2] || tMin[2] > tSecondPoint)
            return false;

        if (tMin[2] > tFirstPoint) tFirstPoint = tMin[2];
        if (tMax[2] < tSecondPoint) tSecondPoint = tMax[2];

        // hit.dPoint1 = tFirstPoint;
        // hit.dPoint2 = tSecondPoint;

        return true;
    }

    bool intersect(const Model& model, Hit& hit, const std::vector<int>& relevantIndices = std::vector<int>()) const {
        const auto& vertices = model.getVertices();
        const auto& indices = model.getIndices();
        const auto& faceNormals = model.getFaceNormals();
        const auto& vertexNormals = model.getVertexNormals();

        // Check if there is an intersection with the AABB
        if (!intersectAABB(model.getAABB()))
            return false;

        // Iterate through each triangle and check if there is an intersection
        bool intersected = false;
        Hit currentHit;

        std::size_t j = 0;
        for (std::size_t i = 0; i < indices.size() + j; i++) {
            if (relevantIndices.size() > 0) {
                if (j == relevantIndices.size())
                    break;
                else
                    i = relevantIndices[j++];
            }

            const Vec3<int>& triangle = indices[i];
            const Vec3<float>& n = faceNormals[i];

            if (intersectTriangle(vertices[triangle[0]], vertices[triangle[1]], vertices[triangle[2]], n, currentHit)
                    && (originModel != &model || originTriangleIndex != (int) i)
                    && (!intersected || currentHit.distance < hit.distance)) {
                hit = currentHit;
                hit.index = i;
                hit.faceNormal = n;

                intersected = true;
            }
        }

        if (intersected) {
            hit.interpolatedNormal = normalize(hit.b0*vertexNormals[indices[hit.index][0]]
                                    + hit.b1*vertexNormals[indices[hit.index][1]]
                                    + hit.b2*vertexNormals[indices[hit.index][2]]);
        }

        return intersected;
    }

private:
    Vec3<float> origin;     // Starting position of the ray
    Vec3<float> direction;  // Direction of the ray
    float epsilon;

    const Model* originModel;
    int originTriangleIndex;
};

#endif