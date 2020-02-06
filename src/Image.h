#ifndef IMAGE_H
#define IMAGE_H

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

	const Vec3<float>& operator() (size_t x, size_t y) const { return img[x + y*width]; }
	Vec3<float>& operator() (size_t x, size_t y) { return img[x + y*width]; }

    void savePPM(const std::string& filename) {
        std::cout << "Image.h" << std::endl;
        std::cout << "      Output file: " << filename << std::endl;

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

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void printInfos() const {
        std::cout << "Image.h" << std::endl;
        std::cout << "      Width:       " << width << std::endl;
        std::cout << "      Height:      " << height << std::endl;
    }

private:
    // 3 floats value per pixel (RGB)
    std::vector<Vec3<float>> img;
    int width;
    int height;
};

#endif