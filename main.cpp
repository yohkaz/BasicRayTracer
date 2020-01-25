#include "Image.h"
#include "Scene.h"
#include "RayTracer.h"
#include "PointLight.h"

void TD1(int width, int height, std::string& filename){
    // Blue color
    Vec3<float> blue(0, 0, 1);
    // White color
    Vec3<float> white(1, 1, 1);

    Image img(width, height);
    img.fillBackgroundY(white, blue);
    img.savePPM(filename);
}

void TD2(int width, int height, std::string& filename) {
    // Blue color
    Vec3<float> blue(0, 0, 1);
    // White color
    Vec3<float> white(1, 1, 1);

    // Define our image
    Image img(width, height);
    img.fillBackgroundY(white, blue);
    // Define our Scene
    Scene scene;

    // Define a triangle model
    std::vector<Vec3<float>> triangle_vertices = {Vec3<float>(0, 0, -2),
                                                  Vec3<float>(-1, 0, -2),
                                                  Vec3<float>(0, -1, -2)};
    std::vector<Vec3<int>> triangle_indices = {Vec3<int>(0, 1, 2)};
    Model triangle(triangle_vertices, triangle_indices);
    // Add triangle to scene
    scene.add(triangle);

    // Define a sphere model
    Model sphere("../models/geometry/sphere2.off");
    // Add triangle to scene
    scene.add(sphere);

    RayTracer::render(img, scene);
    img.savePPM(filename);
}

void TD3(int width, int height, std::string& filename) {
    // Blue color
    Vec3<float> blue(0, 0, 1);
    // White color
    Vec3<float> white(1, 1, 1);

    // Define our image
    Image img(width, height);
    img.fillBackgroundY(white, blue);
    // Define our Scene
    Scene scene;

    // Define a plane model
    std::vector<Vec3<float>> plane_vertices = {Vec3<float>(1.5, -0.5, -1.5),
                                                  Vec3<float>(-1.5, -0.5, -1.5),
                                                  Vec3<float>(-1.5, -0.5, -5.0),
                                                  Vec3<float>(1.5, -0.5, -5.0)};
    std::vector<Vec3<int>> plane_indices = {Vec3<int>(0, 2, 1), Vec3<int>(0, 3, 2)};
    Model plane(plane_vertices, plane_indices);
    plane.setMaterial(Material(Vec3<float>(1.0, 0.0, 0.2), 1.f));
    // Add plane to scene
    scene.add(plane);

    // Define a face model
    Model face("../models/example.off");
    face.setMaterial(Material(Vec3<float>(0.8, 0.4, 0), 1.f));
    // Add face to scene
    scene.add(face);

    // Define a light
    // Vec3<float> lightPos = Vec3<float>(1.f, 1.f, -2.f);
    Vec3<float> lightPos = Vec3<float>(2.f, 2.f, -2.25f);
    Vec3<float> lightColor = Vec3<float>(1.f, 1.f, 1.f);
    float lightIntensity = 1.f;
    scene.add(PointLight(lightPos, lightColor, lightIntensity));

    RayTracer::render(img, scene);
    img.savePPM(filename);
}

void parseArguments(int argc, char *argv[], int& width, int& height, std::string& filename) {
    // Parse arguments
    for(int i = 1; i < argc; i++) {
        std::string currArg(argv[i]);
        if (currArg.compare("-width") == 0) {
            width = strtol(argv[i+1], NULL, 10);
            i++;
        } else if (currArg.compare("-height") == 0) {
            height = strtol(argv[i+1], NULL, 10);
            i++;
        } else if (currArg.compare("-output") == 0) {
            filename = std::string(argv[i+1]);
            i++;
        }
    }
    std::cout << "main.cpp" << std::endl;
    std::cout << "      Width:       " << width << std::endl;
    std::cout << "      Height:      " << height << std::endl;
    std::cout << "      Output file: " << filename << std::endl;
}

int main(int argc, char *argv[]) {
    int width = 600;
    int height = 400;
    std::string filename = "default.ppm";
    parseArguments(argc, argv, width, height, filename);

    // TD1(width, height, filename);
    // TD2(width, height, filename);
    TD3(width, height, filename);
    
    return 0;
}