#include <vector>
#include "Camera.h"
#include "Model.h"

class Scene {
public:
    Scene() = default;

    void add(const Model& model) {
        models.push_back(model);
    }

    const Vec3<float>& getCameraPosition() const {
        return camera.getPosition();
    }
private:
    Camera camera;
    std::vector<Model> models;
};