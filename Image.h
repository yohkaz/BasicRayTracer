#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "Vec3.h"
#include "Scene.h"

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

        #pragma omp for
        for(int j = 0; j < height; j++) {
            // currentColor = (colorBottom - colorTop) * (j/height) + colorTop;
            // currentColor = (colorTop / j) + (colorBottom / (height - j));
            // currentColor = (colorTop)*((height - 1 - j) / ((float) height - 1)) + (colorBottom)*(j / ((float) height - 1));
            currentColor = (colorBottom - colorTop) * (j / ((float) height - 1)) + colorTop;
            // currentColor.normalize();

            for(int i = 0; i < width; i++) {
                img[i + j*width] = currentColor;
            }
        }
    }

    Scene& getScene() {
        return scene;
    }

    void rayTrace() {
        const auto cameraPosition = scene.getCameraPosition();

        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {

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
