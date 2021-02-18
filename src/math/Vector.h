#ifndef OPENCM_VECTOR_H
#define OPENCM_VECTOR_H
#include <cmath>

#define Vec2f Vec2<float>
#define Vec3f Vec3<float>
#define Vec4f Vec4<float>

template <typename T>
class Vec2{
public:
    Vec2(T x, T y) :x(x), y(y) {}
    Vec2() :x(0.0f), y(0.0f) {}
    T x;
    T y;

    T& operator[](const int &index) {
        return (index == 0) ? x : y;
    }

    const Vec2<T>& operator-(const Vec2<T> &rhs) const {
        const Vec2& res = Vec2<T>(x - rhs.x, y - rhs.y);
        return res;
    }

    const Vec2<T>& operator+(const Vec2<T> &rhs) const {
        const Vec2& res = Vec2<T>(x + rhs.x, y + rhs.y);
        return res;
    }

    const Vec2<T>& operator*(const float &arg) const {
        const Vec2& res = Vec2<T>(x * arg, y * arg);
        return res;
    }
};

template <typename T>
class Vec3{
public:
    Vec3(T x, T y, T z) :x(x), y(y), z(z) {}
    Vec3() :x(0.0f), y(0.0f), z(0.0f) {}
    T x;
    T y;
    T z;

    T& operator[](const int &index) {
        return (index == 0) ? x : (index == 1) ? y : z;
    }

    const Vec3<T>& operator-(const Vec3 &v) const {
        const Vec3& res = Vec3<T>(x-v.x, y-v.y, z-v.z);
        return res;
    }

    const Vec3<T>& operator+(const Vec3 &v) const {
        const Vec3& res = Vec3<T>(x+v.x, y+v.y, z+v.z);
        return res;
    }

    const Vec3<T>& operator*(const float &f) const {
        const Vec3& res = Vec3<T>(x*f, y*f, z*f);
        return res;
    }

    const float operator*(const Vec3 &v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    const Vec3<T>& operator/(const Vec3 &v) const {
        const Vec3& res = Vec3<T>(x/v.x, y/v.y, z/v.z);
        return res;
    }

    const Vec3<T>& operator/(const float &v) const {
        const Vec3& res = Vec3<T>(x/v, y/v, z/v);
        return res;
    }

    const Vec3<T>& operator^(const Vec3 &v) const {
        const Vec3& res = Vec3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
        return res;
    }

    void normalize() {
        *this = (*this)*(1.0f/std::sqrt(x*x+y*y+z*z));
    }
};

template <typename T>
class Vec4{
public:
    Vec4(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}
    T r;
    T g;
    T b;
    T a;

    T& operator[](const int &index) {
        return (index == 0) ? r : (index == 1) ? g : (index == 2) ? b : a;
    }
};

#endif
