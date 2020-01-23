#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "Vec3.h"
#include "Scene.h"
#include "Ray.h"

class Image {
public:
    Image(int width, int height): width(width), height(height) {
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                img.push_back(Vec3<float>(0, 0, 0));
            }
        }
    };

    void savePPM(const std::string& filename) {
        std::ofstream PPMfile;
        PPMfile.open(filename);
        PPMfile << "P3" << std::endl;
        PPMfile << width << " " << height << std::endl;
        PPMfile << 255 << std::endl;

        for(int i = 0; i < width*height; i++) {
            PPMfile << (int)(img[i][0]*255) << " ";
            PPMfile << (int)(img[i][1]*255) << " ";
            PPMfile << (int)(img[i][2]*255) << std::endl;
        }

        PPMfile.close();
    }

    void fillBackgroundY(const Vec3<float>& colorTop, const Vec3<float>& colorBottom) {
        Vec3<float> currentColor = colorTop;

        // #pragma omp parallel for
        for(int j = 0; j < height; j++) {
            currentColor = (colorBottom - colorTop) * (j / ((float) height - 1)) + colorTop;

            for(int i = 0; i < width; i++) {
                img[i + j*width] = currentColor;
            }
        }
    }

    Scene& getScene() {
        return scene;
    }

    void rayTrace() {
        const Vec3<float>& cameraPosition = scene.getCamera().getPosition();
        scene.getCamera().printInfos();

        #pragma omp parallel for collapse(2)
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                Vec3<float> pixelPosition = scene.getCamera().computePixelPosition(i, j, width, height);
                Ray ray(cameraPosition, normalize(pixelPosition - cameraPosition));
                float e = -1;
                bool foundHit = false;

                if (i == width-1 && j == height-1)
                    std::cout << "      LastPixelPosition:  " << pixelPosition << std::endl;
                for(const auto& model : scene.getModels()) {
                    Ray::Hit hit;
                    if(ray.intersect(model, hit) && (hit.distance < e || !foundHit)) {
                        foundHit = true;
                        e = hit.distance;
                        img[i + j*width] = Vec3<float>(1.0, 1.0 / (float) model.getVertices().size(), 0);
                    }
                }
            }
        }
    }

private:
    // 3 floats value per pixel (RGB)
    std::vector<Vec3<float>> img;
    int width;
    int height;
    Scene scene;
};

#endif