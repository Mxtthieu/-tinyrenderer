#ifndef OPENCM_MODELLOADER_H
#define OPENCM_MODELLOADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "math/Vector.h"
#include "tgaimage.h"

class ModelLoader {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3f> > faces_;

    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    std::vector<Vec3f> norms_;
    TGAImage normalmap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);

public:
    ModelLoader(const char *filename);
    ~ModelLoader();
    int nverts();
    int nfaces();
    Vec3f normal(int iface, int nthvert);
    Vec3f normal(Vec2f uv);
    Vec3f& vert(int i);
    std::vector<int> face(int idx);

    Vec2f uv(int iface, int nthvert);
    TGAColor diffuse(Vec2f uv);
};


#endif //OPENCM_MODELLOADER_H
