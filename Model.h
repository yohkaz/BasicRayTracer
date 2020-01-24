#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <fstream>
#include <sstream>
#include "Vec3.h"
#include "Material.h"

class Model {
public:
    // Constructors
    Model(std::string filename) {
        std::ifstream file;
        file.open(filename);
        if (file.fail()) {
            std::cout << "Model.h" << std::endl;
            std::cout << "      Fail opening file: " << filename << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line);
        if(line.compare("OFF") == 0) {
            std::cout << "Model.h" << std::endl;
            std::cout << "      Loading OFF file: " << filename << std::endl;
            loadOFF(file);
        }

        computeFaceNormals();
        computeVertexNormals();
    }

    Model(std::vector<Vec3<float>> vertices, std::vector<Vec3<int>> indices):
            vertices(vertices), indices(indices) {
        computeFaceNormals();
        computeVertexNormals();
    }

    // Set functions
    void setMaterial(const Material& m) { material = m; }

    // Get functions
    const std::vector<Vec3<float>>& getVertices() const { return vertices; }
    const std::vector<Vec3<int>>& getIndices() const { return indices; }
    const std::vector<Vec3<float>>& getFaceNormals() const { return faceNormals; }
    const std::vector<Vec3<float>>& getVertexNormals() const { return vertexNormals; }
    const Material& getMaterial() const { return material; }

private:
    void loadOFF(std::ifstream& file) {
        vertices.clear();
        indices.clear();

        int i = 0;
        std::string line;
        int numberOfVertices = -1, numberOfFaces = -1, numberOfEdges = -1;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            if (line.empty() || line[0] == '#') {
                continue;
            } else if (i == 0) {
                iss >> numberOfVertices;
                iss >> numberOfFaces;
                iss >> numberOfEdges;
                std::cout << "      #vertices: " << numberOfVertices << std::endl;
                std::cout << "      #faces: " << numberOfFaces << std::endl;
                std::cout << "      #edges: " << numberOfEdges << std::endl;
            } else if (numberOfVertices-- > 0) {
                float x, y, z;
                if (!(iss >> x >> y >> z))
                    break;  // error
                z -= 3;
                vertices.push_back(Vec3<float>(x, y, z));
            } else if (numberOfFaces-- > 0) {
                int numberOfIndices;
                int i1, i2, i3; // suppose it's 3

                if (!(iss >> numberOfIndices))
                    break;  // error
                if (numberOfIndices != 3) {
                    std::cout << "      Wrong number of vertices by face: " << numberOfIndices << std::endl;
                    break;
                }
                if (!(iss >> i1 >> i2 >> i3))
                    break;  // error

                indices.push_back(Vec3<int>(i1, i2, i3));
            }

            i++;
        }
    }

    void computeCentroid() {
        centroid = Vec3<float>(0, 0, 0);
        for (auto& v : vertices)
            centroid = centroid + v;

        centroid /= vertices.size();
    }

    void reorientFaceNormals() {
        for (std::size_t i=0; i < indices.size(); i++) {
        // for (const auto& face : indices) {
            const auto& v0 = vertices[indices[i][0]];
            const auto& v1 = vertices[indices[i][1]];
            const auto& v2 = vertices[indices[i][2]];
            Vec3<float> faceMid = (v0 + v1 + v2) / 3;

            double distToCentroid = (faceMid - centroid).length();
            double distWithNormal = ((faceMid + faceNormals[i]) - centroid).length();

            if (distWithNormal < distToCentroid)
                faceNormals[i] = faceNormals[i] * -1;
        }
    }

    void computeFaceNormals() {
        // computeCentroid();
        faceNormals.clear();
        for (const auto& face : indices) {
            const auto& v0 = vertices[face[0]];
            const auto& v1 = vertices[face[1]];
            const auto& v2 = vertices[face[2]];

            Vec3<float> e1 = v1 - v0;
            Vec3<float> e2 = v2 - v0;

            faceNormals.push_back(normalize(cross(e1, e2)));
        }
        // reorientFaceNormals();
    }

    void computeVertexNormals() {
        // vertex -> faces in which the vertex is part of
        std::vector<std::vector<int>> vToF(vertices.size());
        for (std::size_t i=0; i < indices.size(); i++) {
            vToF[indices[i][0]].push_back(i);
            vToF[indices[i][1]].push_back(i);
            vToF[indices[i][2]].push_back(i);
        }

        // compute vertex normals
        for (std::size_t i = 0; i < vertices.size(); i++) {
            const std::vector<int>& vertexFaces = vToF[i];
            Vec3<float> vNormal(0.0, 0.0, 0.0);

            for (int fIdx : vertexFaces)
                vNormal = vNormal + faceNormals[fIdx];

            if (vertexFaces.size() == 0 || vNormal.length() == 0) {
                vNormal = vNormal + Vec3<float>(1.0, 1.0, 1.0);
                vNormal.normalize();
                vertexNormals.push_back(vNormal);
                continue;
            }

            vNormal = vNormal / vertexFaces.size();
            vNormal.normalize();
            vertexNormals.push_back(vNormal);
        }
    }

    std::vector<Vec3<float>> vertices;          // vertices positions
    std::vector<Vec3<float>> vertexNormals;   // normal of the vertices
    std::vector<Vec3<int>> indices;             // each Vec3 contain the indices of a triangle
    std::vector<Vec3<float>> faceNormals;       // normal of the faces
    Vec3<float> centroid;
    Material material;
};

#endif