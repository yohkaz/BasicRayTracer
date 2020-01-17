#include <vector>
#include "Vec3.h"

class Model {
public:
    Model(std::vector<Vec3<float>> vertices, std::vector<int> indexes):
        vertices(vertices), indexes(indexes) {}

private:
    std::vector<Vec3<float>> vertices;
    std::vector<int> indexes;
};