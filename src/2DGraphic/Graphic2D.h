#ifndef OPENCM_GRAPHIC2D_H
#define OPENCM_GRAPHIC2D_H


#include <vector>
#include "../math/Vector.h"
#include "../ModelLoader.h"
#include "../math/Matrix.h"
#include <fstream>

class Graphic2D {

public:
    Graphic2D();
    void drawPixel(int x, int y, Vec3f color);
    void drawPixelStereo(int x, int y, Vec3f color, int img);
    void drawLine(int x0, int y0, int x1, int y1, Vec3f color);
    void drawEmptyTriangle(Vec2f t0, Vec2f t1, Vec2f t2, Vec3f color);
    void drawTriangle(Vec3f *pts, Vec3f light_dir, Matrix uvs, ModelLoader model);
    void drawTriangleStereo(Vec3f *pts, Vec3f color, int img, Matrix uvs, ModelLoader model);
    void save();
    ~Graphic2D();

    const int width    = 800;
    const int height   = 800;
private:
    Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P);
    std::vector<Vec3f*> framebuffer;
    float* zbuffer;
    float* zbuffer2;
};

#endif
