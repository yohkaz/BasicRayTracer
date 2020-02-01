#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Camera.h"
#include "Model.h"
#include "Light.h"

class Scene {
public:
    Scene() = default;

    void add(Model& model) {
        models.push_back(&model);
    }

    void add(Light& light) {
        ligths.push_back(&light);
    }

    const Camera& getCamera() const { return camera; }
    const std::vector<Model*>& getModels() const { return models; }
    const std::vector<Light*>& getLights() const { return ligths; }
    const Vec3<float>& getCameraPosition() const { return camera.getPosition(); }

private:
    Camera camera;
    std::vector<Model*> models;
    std::vector<Light*> ligths;
};

#endif