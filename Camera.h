#include "Vec3.h"

class Camera {
public:
    Camera(): position(Vec3<float>(0, 0, 0)), orientation(Vec3<float>(0, 0, -1)), fieldOfView(45), aspectRatio(3/2) {}

    void setPosition(const Vec3<float>& p)    { position = p; }
    void setOrientation(const Vec3<float>& o) { orientation = o; }
    void setFieldOfView(float fov)            { fieldOfView = fov; }
    void setAspectRation(float ar)            { aspectRatio = ar; }

    const Vec3<float>& getPosition() const { return position; }
    const Vec3<float>& getOrientation() const { return orientation; }

private:
    Vec3<float> position;
    Vec3<float> orientation;
    float fieldOfView;          // angle in °
    float aspectRatio;          // width/height
};