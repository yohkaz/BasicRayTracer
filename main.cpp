#include "Image.h"

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

    // Define a triangle model
    std::vector<Vec3<float>> triangle_vertices = {Vec3<float>(0, 0, -1),
                                                  Vec3<float>(1, 0, -1),
                                                  Vec3<float>(0, 1, -1)};
    std::vector<int> triangle_indexes = {0, 1, 2};
    Model triangle(triangle_vertices, triangle_indexes);

    // Add triangle to scene
    img.getScene().add(triangle);
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
    std::cout << "Width:       " << width << std::endl;
    std::cout << "Height:      " << height << std::endl;
    std::cout << "Output file: " << filename << std::endl;
}

int main(int argc, char *argv[]) {
    int width = 200;
    int height = 200;
    std::string filename = "default.ppm";
    parseArguments(argc, argv, width, height, filename);

    TD1(width, height, filename);
    // TD2(width, height, filename);
    
    return 0;
}