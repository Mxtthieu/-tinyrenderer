#include <iostream>
#include "Graphic2D.h"
#include "../ModelLoader.h"
#include <limits>

Graphic2D::Graphic2D() : framebuffer(width*height), zbuffer(new float[width*height]),  zbuffer2(new float[width*height]){
    // framebuffer initialisation
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            framebuffer[i+j*width] = new Vec3f(0.f,0.f, 0.f);
        }
    }

    // zbuffers initialisation
    for (int k = 0; k < width*height; ++k) {
        zbuffer[k] = std::numeric_limits<float>::max();
    }
    for (int k = 0; k < width*height; ++k) {
        zbuffer2[k] = std::numeric_limits<float>::max();
    }
}

void Graphic2D::save() {
    // save the framebuffer to ppm file
    std::ofstream ofs;
    ofs.open("./out.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.0f, std::min(1.f, (*framebuffer[i])[j])));
        }
    }
    ofs.close();
}

Graphic2D::~Graphic2D() {
    // free framebuffer
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            delete(framebuffer[i+j*width]);
        }
    }

    // free zbuffers
    free(zbuffer);
    free(zbuffer2);
}

void Graphic2D::drawLine(int x0, int y0, int x1, int y1, Vec3f color) {
    //---------------
    // Verification
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) { // Verify if angle is too important
        // if too important swap x and y to have to compute a line going left to right
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) { // Verify if second point is on the right of the first point
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    //---------------
    // End Verification



    //---------------
    // Draw

    // Compute movement coefficient
    int dx = x1-x0;
    int dy = y1-y0;

    // Optimisation to not have to compute division every time
    int derror2 = std::abs(dy)*2;
    int error2 = 0;

    // Start drawing
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            drawPixel(y, x, color);
        } else {
            drawPixel(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
    //---------------
    // End Draw
}

void Graphic2D::drawPixel(int x, int y, Vec3f color) {
    y = height - y; // Inverse image

    // Check if position is correct
    if (x+y*width < width*height) {
        framebuffer[x+y*width]->x = color.x;
        framebuffer[x+y*width]->y = color.y;
        framebuffer[x+y*width]->z = color.z;
    }
}

void Graphic2D::drawEmptyTriangle(Vec2f t0, Vec2f t1, Vec2f t2, Vec3f color) {
    drawLine(t0.x, t0.y, t1.x, t1.y, color);
    drawLine(t1.x, t1.y, t2.x, t2.y, color);
    drawLine(t2.x, t2.y, t0.x, t0.y, color);
}

void Graphic2D::drawTriangle(Vec3f *pts, Vec3f light_dir, Matrix uvs, ModelLoader model) {

    // Compute the interval of value for all the point in the triangle
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(width-1, height-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }


    // For all points in the triangle
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {

            // Compute barycentre of the actual triangle
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);

            // Verify if triangle is not degenerate
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;

            // Compute z value of the specific point for zbuffer
            P.z = 0;
            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];

            // Zbuffer verification
            if (zbuffer[int(P.x+P.y*width)]> - P.z) {
                zbuffer[int(P.x+P.y*width)] = -P.z;


                //---------------
                // Ambient
                Vec3f bc_clip    = Vec3f(bc_screen.x/pts[0][1], bc_screen.y/pts[1][1], bc_screen.z/pts[2][1]);
                bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z);

                // Compute uv coordination
                Vec2f uv;
                for (int j = 0; j < 2 ; j++) {
                    Vec3f bc(uvs[j][0], uvs[j][1], uvs[j][2]);
                    float test = bc * bc_clip;

                    uv[j] = test;
                }

                TGAColor ambient = model.diffuse(uv);
                Vec3f c = Vec3f((float)ambient[2] / 255.f, (float)ambient[1] / 255.f, (float)ambient[0] / 255.f);
                //---------------
                // End Ambient


                //---------------
                // End Diffuse

                // Useful variables
                float lightIntensity = 2.f;

                Vec3f normal = model.normal(uv);
                float diffuse = (normal * light_dir);
                diffuse /= lightIntensity; // less light intensity
                if (diffuse < 0) { diffuse = -diffuse;} // Inverse light direction
                //---------------
                // End Diffuse


                //---------------
                // Specular

                // Useful variables
                float specularStrength = 1.f/2500.f;
                Vec3f viewDir = P - Vec3f(0, 0, 3);
                viewDir.normalize();
                Vec3f lightDir = Vec3f(0, 0, 1);

                // Compute reflect direction vector
                Vec3f lightDirMT = Vec3f(lightDir.x, lightDir.y - 10.f, lightDir.z + 100.f);
                float dot = normal * light_dir;
                Vec3f first = lightDirMT * dot;
                Vec3f reflectDir = Vec3f(first.x * normal.x, first.y * normal.y, first.z * normal.z);

                // Compute specular effect
                float spec = pow(fmax(viewDir * reflectDir, 0.0), 4);
                float specular = specularStrength * spec;
                specular /= lightIntensity;
                //---------------
                // End Specular


                // Compute all effects and draw
                float effect = std::max(0.5f, diffuse + specular);
                c = c * effect;
                drawPixel(P.x, P.y,  c);
            }
        }
    }
}

Vec3f Graphic2D::barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    // Computes coordinates of a point P in the given triangle ABC

    /*  Triangle can be describe like P = alpha A + beta B + gamma C
     *  with alpha beta and gama the coordinate of barycentre
    */

    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = s[0] ^ s[1];
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void Graphic2D::drawTriangleStereo(Vec3f *pts, Vec3f light_dir, int img, Matrix uvs, ModelLoader model) {
    // Compute the interval of value for all the point in the triangle
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(width-1, height-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }


    // For all points in the triangle
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {

            // Compute barycentre of the actual triangle
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);

            // Verify if triangle is not degenerate
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;

            // Compute z value of the specific point for zbuffer
            P.z = 0;
            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
                //---------------
                // Ambient
                Vec3f bc_clip    = Vec3f(bc_screen.x/pts[0][1], bc_screen.y/pts[1][1], bc_screen.z/pts[2][1]);
                bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z);

                // Compute uv coordination
                Vec2f uv;
                for (int j = 0; j < 2 ; j++) {
                    Vec3f bc(uvs[j][0], uvs[j][1], uvs[j][2]);
                    float test = bc * bc_clip;

                    uv[j] = test;
                }

                TGAColor ambient = model.diffuse(uv);
                Vec3f c = Vec3f((float)ambient[2] / 255.f, (float)ambient[1] / 255.f, (float)ambient[0] / 255.f);
                //---------------
                // End Ambient


                //---------------
                // End Diffuse

                // Useful variables
                float lightIntensity = 2.f;

                Vec3f normal = model.normal(uv);
                float diffuse = (normal * light_dir);
                diffuse /= lightIntensity; // less light intensity
                if (diffuse < 0) { diffuse = -diffuse;} // Inverse light direction
                //---------------
                // End Diffuse


                //---------------
                // Specular

                // Useful variables
                float specularStrength = 1.f/2500.f;
                Vec3f viewDir = P - Vec3f(0, 0, 3);
                viewDir.normalize();
                Vec3f lightDir = Vec3f(0, 0, 1);

                // Compute reflect direction vector
                Vec3f lightDirMT = Vec3f(lightDir.x, lightDir.y - 10.f, lightDir.z + 100.f);
                float dot = normal * light_dir;
                Vec3f first = lightDirMT * dot;
                Vec3f reflectDir = Vec3f(first.x * normal.x, first.y * normal.y, first.z * normal.z);

                // Compute specular effect
                float spec = pow(fmax(viewDir * reflectDir, 0.0), 4);
                float specular = specularStrength * spec;
                specular /= lightIntensity;
                //---------------
                // End Specular


                // Compute all effects and draw
                float effect = std::max(0.5f, diffuse + specular);
                c = c * effect;

                if (img == 0) {
                    // Zbuffer verification
                    if (zbuffer[int(P.x+P.y*width)]>-P.z) {
                        zbuffer[int(P.x+P.y*width)] = -P.z;
                        drawPixelStereo(P.x, P.y,  c, img);
                    }
                }
                else {
                    // Zbuffer verification
                    if (zbuffer2[int(P.x+P.y*width)]>-P.z) {
                        zbuffer2[int(P.x+P.y*width)] = -P.z;
                        drawPixelStereo(P.x, P.y,  c, img);
                    }
                }
            }
        }
}

void Graphic2D::drawPixelStereo(int x, int y, Vec3f color, int img) {
    y = height - y; // inverse image

    // Check if position is correct
    if (x+y*width < width*height) {
        if (img == 0) { framebuffer[x+y*width]->z = (color.x + color.y + color.z) / 3.0; }
        else { framebuffer[x+y*width]->x = (color.x + color.y + color.z) / 3.0; }
        framebuffer[x+y*width]->y = 0.0f;
    }
}
