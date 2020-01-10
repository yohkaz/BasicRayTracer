#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Vec3.h"

class Image {
public:
    Image(int width, int height): width(width), height(height) {
        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                img.push_back(Vec3<float>(0, 0, 0));
            }
        }
    };

    void savePPM(std::string filename) {
        std::ofstream PPMfile;
        PPMfile.open(filename);
        PPMfile << "P3" << std::endl;
        PPMfile << width << " " << height << std::endl;
        PPMfile << 255 << std::endl;

        for(int i = 0; i < width*height; i++) {
            PPMfile << (int)(img[i].v0*255) << " ";
            PPMfile << (int)(img[i].v1*255) << " ";
            PPMfile << (int)(img[i].v2*255) << std::endl;
        }

        PPMfile.close();
    }

    void fillBackgroundY(Vec3<float>& colorTop, Vec3<float>& colorBottom) {
        Vec3<float> currentColor = colorTop;

        #pragma omp for
        for(int j = 0; j < height; j++) {
            if (j > 0) {
                // currentColor = (colorBottom - colorTop) * (j/height) + colorTop;
                currentColor = (colorTop / j) + (colorBottom / (height - j));
                currentColor.normalize();
            }

            for(int i = 0; i < width; i++) {
                img[i + j*width] = currentColor;
            }
        }
    }

private:
    // 3 floats value per pixel (RGB)
    std::vector<Vec3<float>> img;
    int width;
    int height;
};
