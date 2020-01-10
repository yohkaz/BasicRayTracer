#include <math.h>

template <typename T>
class Vec3 {
    public:
    T v0;
    T v1;
    T v2;
    
    Vec3() {}
    Vec3(T in0, T in1, T in2) : v0(in0), v1(in1), v2(in2) {}
    Vec3 operator-(const Vec3& v) const { return Vec3(v0 - v.v0, v1 - v.v1, v2 - v.v2); }
    Vec3 operator+(const Vec3& v) const { return Vec3(v0 + v.v0, v1 + v.v1, v2 + v.v2); }
    Vec3 operator*(double d) const { return Vec3(v0*d, v1*d, v2*d); }
    Vec3 operator/(double d) const { return Vec3(v0/d, v1/d, v2/d); }
    Vec3 cross(const Vec3& v) const { return Vec3(v1*v.v2 - v2*v.v1, v2*v.v0 - v0*v.v2, v0*v.v1 - v1*v.v0); }
    Vec3 dot(const Vec3& v) const { return Vec3(v0*v.v0, v1*v.v1, v2*v.v2); }
    double length() { return sqrt((v0 * v0) + (v1 * v1) + (v2 * v2)); }
    void normalize() { double len = length(); v0 /= len; v1 /= len; v2 /= len; }
};
