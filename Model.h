#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <fstream>
#include <sstream>
#include "Vec3.h"

class Model {
public:
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
            readOFF(file);
        }

        computeFaceNormals();
    }

    Model(std::vector<Vec3<float>> vertices, std::vector<Vec3<int>> indices):
            vertices(vertices), indices(indices) {
        computeFaceNormals();
    }

    const std::vector<Vec3<float>>& getVertices() const { return vertices; }
    const std::vector<Vec3<int>>& getIndices() const { return indices; }
    const std::vector<Vec3<float>>& getFaceNormals() const { return faceNormals; }
private:
    void readOFF(std::ifstream& file) {
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
                z -= 4;
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

    void computeFaceNormals() {
        faceNormals.clear();
        for (const auto& face : indices) {
            const auto& v0 = vertices[face[0]];
            const auto& v1 = vertices[face[1]];
            const auto& v2 = vertices[face[2]];

            Vec3<float> e1 = v1 - v0;
            Vec3<float> e2 = v2 - v0;

            faceNormals.push_back(normalize(cross(e1, e2)));
        }
    }

    std::vector<Vec3<float>> vertices;      // vertices positions
    std::vector<Vec3<int>> indices;         // each Vec3 contain the indices of a triangle
    std::vector<Vec3<float>> faceNormals;   // normal of the faces
};

#endif