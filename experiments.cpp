#include "src/Image.h"
#include "src/Scene.h"
#include "src/RayTracer.h"
#include "src/PointLight.h"
#include "src/AreaLight.h"

void learningScene1(int width, int height, std::string& filename) {
    // bottom color
    Vec3<float> bottom(0.15, 0.15, 0.15);
    // top color
    Vec3<float> top(0, 0, 0);

    // Define our image
    Image img(width, height);
    img.printInfos();
    img.fillBackgroundY(bottom, top);

    // Define our Scene
    Scene scene;

    // Define the walls
    Model walls("../models/walls1.off");
    Material wallsMaterial(Vec3<float>(0.8, 0.8, 0.8), 1.f, 0.4f);
    walls.setMaterial(wallsMaterial);
    // Add plane to scene
    //walls.scale(Vec3<float>(1.2f, 1.5f, 1.2f));
    walls.translate(Vec3<float>(0.f, -1.7f, -4.2f));
    scene.add(walls);

    // Define small obj
    Model smallObj("../models/smallcube.off");
    Material smallObjMaterial(Vec3<float>(0.95, 0.55, 0.55), 0.6f, 0.40f, 0.5f, 0.2f);
    smallObj.setMaterial(smallObjMaterial);
    smallObj.scale(Vec3<float>(0.5f, 0.5f, 0.5f));
    smallObj.translate(Vec3<float>(-0.8f, -1.2f, -4.0f));
    scene.add(smallObj);

    // Define big obj
    Model bigObj("../models/rectangle.off");
    Material bigObjMaterial(Vec3<float>(0.55, 0.55, 0.95), 0.7f, 0.40f, 0.5f, 0.2f);
    bigObj.setMaterial(bigObjMaterial);
    bigObj.scale(Vec3<float>(0.5f, 0.5f, 0.5f));
    bigObj.translate(Vec3<float>(0.7f, -1.7f, -4.8f));
    scene.add(bigObj);

    // Define a emitting model
    Model plane("../models/simplecube.off");
    plane.scale(Vec3<float>(2.0f, 0.02f, 2.0f));
    plane.translate(Vec3<float>(0.0f, 2.3f, -5.0f));
    Material planeMaterial(Vec3<float>(1.0, 1.0, 1.0), 1.f, 0.1f, 0.0f, 1.0f);
    plane.setMaterial(planeMaterial);
    // Add plane to scene
    scene.add(plane);

    RayTracer rayTracer;
    rayTracer.enableBVH(10);
    //rayTracer.enagleCosineWeighted();
    rayTracer.enableLearningLT();
    rayTracer.enablePathTracing(3, 144);
    rayTracer.printInfos();
    rayTracer.render(img, scene);
    img.savePPM(filename);
}

void learningScene2(int width, int height, std::string& filename) {
    // bottom color
    Vec3<float> bottom(0.15, 0.15, 0.15);
    // top color
    Vec3<float> top(0, 0, 0);

    // Define our image
    Image img(width, height);
    img.printInfos();
    img.fillBackgroundY(bottom, top);

    // Define our Scene
    Scene scene;

    // Define the walls
    Model walls("../models/walls1.off");
    Material wallsMaterial(Vec3<float>(1., 1., 1.), 1.f, 0.4f);
    walls.setMaterial(wallsMaterial);
    walls.translate(Vec3<float>(0.f, -1.7f, -4.2f));
    scene.add(walls);

    // Define small obj
    Model smallObj("../models/smallcube.off");
    Material smallObjMaterial(Vec3<float>(1., 0.65, 0.65), 1.f, 0.40f);
    smallObj.setMaterial(smallObjMaterial);
    smallObj.scale(Vec3<float>(0.5f, 0.5f, 0.5f));
    smallObj.translate(Vec3<float>(-0.8f, -1.2f, -4.0f));
    scene.add(smallObj);

    // Define big obj
    Model bigObj("../models/rectangle.off");
    Material bigObjMaterial(Vec3<float>(0.65, 0.65, 1.), 1.f, 0.40f);
    bigObj.setMaterial(bigObjMaterial);
    bigObj.scale(Vec3<float>(0.5f, 0.5f, 0.5f));
    bigObj.translate(Vec3<float>(0.7f, -1.7f, -4.8f));
    scene.add(bigObj);

    // Define a emitting model
    Model plane("../models/simplecube.off");
    plane.scale(Vec3<float>(1.6f, 0.02f, 2.0f));
    plane.translate(Vec3<float>(-2.2f, 2.3f, -5.0f));
    Material planeMaterial(Vec3<float>(1.0, 1.0, 1.0), 1.f, 0.1f, 0.0f, 1.0f);
    plane.setMaterial(planeMaterial);
    // Add plane to scene
    scene.add(plane);

    // Ray tracer parameters
    RayTracer rayTracer;
    rayTracer.enableBVH(5);
    //rayTracer.enagleCosineWeighted();
    rayTracer.enableLearningLT();
    rayTracer.enablePathTracing(3, 144);
    rayTracer.printInfos();
    rayTracer.render(img, scene);
    img.savePPM(filename);
}

int runExperiments(int argc, char *argv[]) {
    int width = 450;
    int height = 300;
    std::string filename1 = "scene1.ppm";
    std::string filename2 = "scene2.ppm";

    learningScene1(width, height, filename1);
    learningScene2(width, height, filename2);

    return 0;
}
