#ifndef RAY_H
#define RAY_H

#include "Vec3.h"
#include "Model.h"

class Ray {
public:
    class Hit {
        public:
        int index;              // index of the triangle
        float distance;         // distance of the hit
        Vec3<float> faceNormal; // face normal of the triangle

        // vertice normals
        Vec3<float> vNormal0;
        Vec3<float> vNormal1;
        Vec3<float> vNormal2;

        // barycentric coordinates
        float b0;
        float b1;
        float b2;
    };

    Ray(const Vec3<float>& origin, const Vec3<float>& direction): origin(origin), direction(direction), epsilon(0.00001f) {}

    // bool intersectTriangle(const Vec3<float>& p0,
    //                         const Vec3<float>& p1,
    //                         const Vec3<float>& p2,
    //                         const Vec3<float>& n,
    //                         Hit& hit) const {
    //     Vec3<float> e1 = p1 - p0; // e0
    //     Vec3<float> e2 = p2 - p0; // e1

    //     Vec3<float> q = cross(direction, e2);
    //     float a = dot(e1, q);
    //     if (std::abs(a) < epsilon)
    //         return false;

    //     Vec3<float> s = (origin - p0) / a;
    //     Vec3<float> r = cross(s, e1);
    //     float b0 = dot(s, q);
    //     float b1 = dot(r, direction);
    //     float b2 = 1 - b0 - b1;
    //     if (b0 < 0 || b1 < 0 || b2 < 0)
    //         return false;

    //     float distance = dot(e2, r);
    //     if (distance >= 0) {
    //         hit.distance = distance;
    //         hit.b0 = b0;
    //         hit.b1 = b1;
    //         hit.b2 = b2;
    //         return true;
    //     }

    //     return false;
    // }
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
        if (hit.b1 < 0.f || hit.b1 > 1.f)
            return false;
        if (hit.b2 >= 0.f && hit.b1 + hit.b2 <= 1.f)
            return true;
        return false;
    }

    bool intersect(const Model& model, Hit& hit) const {
        const auto& vertices = model.getVertices();
        const auto& indices = model.getIndices();
        const auto& faceNormals = model.getFaceNormals();
        const auto& vertexNormals = model.getVertexNormals();

        // Iterate through each triangle and check if there is an intersection
        bool intersected = false;
        Hit currentHit;
        for (std::size_t i = 0; i < indices.size(); i++) {
            const Vec3<int>& triangle = indices[i];
            const Vec3<float>& n = faceNormals[i];

            if (intersectTriangle(vertices[triangle[0]], vertices[triangle[1]], vertices[triangle[2]], n, currentHit)
                    && (!intersected || currentHit.distance < hit.distance)) {
                hit = currentHit;
                hit.index = i;
                hit.faceNormal = n;
                hit.vNormal0 = vertexNormals[triangle[0]];
                hit.vNormal1 = vertexNormals[triangle[1]];
                hit.vNormal2 = vertexNormals[triangle[2]];

                intersected = true;
            }
        }

        return intersected;
    }

private:
    Vec3<float> origin;     // Starting position of the ray
    Vec3<float> direction;  // Direction of the ray
    float epsilon;
};

#endif