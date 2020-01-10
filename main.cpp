#include "Image.h"

int main(int argc, char *argv[]) {
    int width;
    int height;
    std::string filename;

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
    
    // Blue color
    Vec3<float> blue(0, 0, 1);
    // White color
    Vec3<float> white(1, 1, 1);

    Image img(width, height);
    img.fillBackgroundY(white, blue);    
    img.savePPM(filename);

    return 0;
}