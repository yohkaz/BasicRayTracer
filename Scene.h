#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Camera.h"
#include "Model.h"

class Scene {
public:
    Scene() = default;

    void add(const Model& model) {
        models.push_back(model);
    }

    const Camera& getCamera() const { return camera; }
    const std::vector<Model>& getModels() const { return models; }
    const Vec3<float>& getCameraPosition() const { return camera.getPosition(); }
private:
    Camera camera;
    std::vector<Model> models;
};

#endif