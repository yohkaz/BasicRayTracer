#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include "Vec3.h"

#define PI 3.14159265f

class Camera {
public:
    Camera(): position(Vec3<float>(0, 0, 0)), direction(Vec3<float>(0, 0, -1)), distanceToPlane(0.2), fieldOfView(45), aspectRatio(3/2.0f) {
        // Compute width & height from the aspect ratio + distance + fov
        height = 2.0f * distanceToPlane * tan((fieldOfView * PI) / (180.0 * 2.0));
        width = height * aspectRatio;

        // Compute camera coordinate system
        n = -normalize(direction);
        up = Vec3<float>(0, 1, 0);
        right = normalize(cross(up, n));
        up = normalize(cross(n, right));

        topLeftPixelPosition = ((position - n)*distanceToPlane) - right*(width/2.0f) + up*(height/2.0f);
        printInfos();
    }

    // Set functions
    void setPosition(const Vec3<float>& p) { position = p; }
    void setDirection(const Vec3<float>& o) { direction = o; }
    void setFieldOfView(float fov) { fieldOfView = fov; }
    void setAspectRation(float ar) { aspectRatio = ar; }

    // Get functions
    const Vec3<float>& getPosition() const { return position; }
    const Vec3<float>& getDirection() const { return direction; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    Vec3<float> computePixelPosition(float x, float y) const {
        return topLeftPixelPosition + (right* x * width) - (up * y * height);
    }

    void printInfos() const {
        std::cout << "Camera.h   " << std::endl;
        std::cout << "      Position:   " << position << std::endl;
        std::cout << "      Direction:  " << direction << std::endl;
        std::cout << "      Height:     " << height << std::endl;
        std::cout << "      Width:      " << width << std::endl;
        std::cout << "      n:          " << n << std::endl;
        std::cout << "      up:         " << up << std::endl;
        std::cout << "      right:      " << right << std::endl;
        std::cout << "      topLeftPixelPosition:    " << topLeftPixelPosition << std::endl;
    }

private:
    Vec3<float> position;       // position of the camera
    Vec3<float> direction;      // direction of the camera
    float distanceToPlane;      // distance of the camera to the image plane
    float fieldOfView;          // angle in °
    float aspectRatio;          // width/height
    float width;
    float height;

    // Camera coordinate system
    Vec3<float> up;
    Vec3<float> right;
    Vec3<float> n;              // normal to the viewing plane
    Vec3<float> topLeftPixelPosition;
};

#endif