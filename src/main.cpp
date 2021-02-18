#include "2DGraphic/Graphic2D.h"
#include "ModelLoader.h"
#include "math/Matrix.h"

const int depth  = 255;

Vec3f matrixToVector(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix vectorToMatrix(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}


Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}


void render() {
    Graphic2D context2D;

    ModelLoader *model = new ModelLoader("diablo.obj");


    Vec3f pts[3];
    Vec3f pts2[3];
    Vec3f light_dir(0, 0, -3);
    Vec3f camera(0,0,3);

    Matrix Projection(4);
    Matrix ViewPort   = viewport(context2D.width/8 - 20.f, context2D.height/8, context2D.width*3/4, context2D.height*3/4);
    Matrix ViewPort2   = viewport(context2D.width/8 + 20.f, context2D.height/8, context2D.width*3/4, context2D.height*3/4);
    Projection[3][2] = -1.f/camera.z;






    /* TEST 0 */
    /*context2D.drawLine(0, 0, 200, 200, Vec3f(1.0f, 1.0f, 1.0f));*/






    /* TEST 1 FULL TRIANGLE */
    /*pts[0] = Vec3f(10, 70, 1);
    pts[1] = Vec3f(50, 160, 1);
    pts[2] = Vec3f(70, 80, 1);
    context2D.drawTriangle(pts, Vec3f(1.0f, 0.0f, 0.0f));
    pts[0] = Vec3f(180, 50, 1);
    pts[1] = Vec3f(150, 1, 1);
    pts[2] = Vec3f(70, 180, 1);
    context2D.drawTriangle(pts, Vec3f(0.0f, 0.0f, 1.0f));
    pts[0] = Vec3f(180, 150, 1);
    pts[1] = Vec3f(120, 160, 1);
    pts[2] = Vec3f(130, 180, 1);
    context2D.drawTriangle(pts, Vec3f(0.0f, 1.0f, 0.0f));*/






    /* TEST 2 MULTICOLOR FACE*/
    /*for (int i=0; i<model->nfaces(); i++) {
        Vec3f pts[3];
        std::vector<int> face = model->face(i);
        Vec2f screen_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f world_coords = model->vert(face[j]);
            screen_coords[j] = Vec2f((world_coords.x+1.)*context2D.width/2., (world_coords.y+1.)*context2D.height/2.);
            pts[j] = Vec3f(int(screen_coords[j].x), int(screen_coords[j].y), model->vert(face[j]).z);
        }
        context2D.drawTriangle(pts, Vec3f(rand()%255/255.0f, rand()%255/255.0f, rand()%255/255.0f));
    }*/






    /* TEST 3 TRIANGLE FACE */
    /*for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*context2D.width/2.;
            int y0 = (v0.y+1.)*context2D.height/2.;
            int x1 = (v1.x+1.)*context2D.width/2.;
            int y1 = (v1.y+1.)*context2D.height/2.;
            context2D.drawLine(x0, y0, x1, y1, Vec3f(1.0f, 1.0f, 1.0f));
        }
    }*/






    /* TEST 4 ORTHOGRAPHIC */
    /*for (int i=0; i<model->nfaces(); i++) {
        Vec3f pts[3];
        std::vector<int> face = model->face(i);
        Vec2f screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2f((v.x+1.)*(context2D.width/2.), (v.y+1.)*(context2D.height/2.));
            world_coords[j]  = v;

            Vec3f pos = Vec3f(int(((model->vert(face[j])).x+1.)*(context2D.width/2.)+.5), int(((model->vert(face[j])).y+1.)*(context2D.height/2.)+.5), (model->vert(face[j])).z);
            pts[j] = pos;

        }
        Vec3f n1 = (world_coords[2] - world_coords[0]);
        Vec3f n2 = (world_coords[1] - world_coords[0]);
        Vec3f n = n1^n2;


        n.normalize();
        float intensity = n*light_dir;
        if (intensity>0) {
            context2D.drawTriangle(pts, Vec3f(intensity, intensity, intensity));
        }
    }*/






    /* TEST 5 PERSPECTIVE */
    /*// For all faces in the obj
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];

        // Compute screen coordinate
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = matrixToVector(ViewPort * Projection * vectorToMatrix(v));
            world_coords[j]  = v;
            Vec3f pos = Vec3f(int(screen_coords[j].x), int(screen_coords[j].y), (model->vert(face[j])).z);
            pts[j] = pos;
        }

        // Normal from normal map
        Matrix uvs(2, 3);
        uvs.set_col(0, model->uv(i, 0));
        uvs.set_col(1, model->uv(i, 1));
        uvs.set_col(2, model->uv(i, 2));

        // Draw
        context2D.drawTriangle(pts, light_dir, uvs, *model);
    }*/



    /* TEST 6 PERSPECTIVE 3D */
    // For all faces in the obj
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f screen_coords2[3];
        Vec3f world_coords[3];

        // Compute screen coordinate
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] =  matrixToVector(ViewPort*Projection*vectorToMatrix(v));
            screen_coords2[j] =  matrixToVector(ViewPort2*Projection*vectorToMatrix(v));
            world_coords[j]  = v;
            Vec3f pos = Vec3f(int(screen_coords[j].x), int(screen_coords[j].y), (model->vert(face[j])).z);
            Vec3f pos2 = Vec3f(int(screen_coords2[j].x), int(screen_coords2[j].y), (model->vert(face[j])).z);
            pts[j] = pos;
            pts2[j] = pos2;
        }

        // Normal from normal map
        Matrix uvs(2, 3);
        uvs.set_col(0, model->uv(i, 0));
        uvs.set_col(1, model->uv(i, 1));
        uvs.set_col(2, model->uv(i, 2));

        // Draw
        context2D.drawTriangleStereo(pts, light_dir, 0, uvs, *model);
        context2D.drawTriangleStereo(pts2, light_dir, 1,  uvs, *model);
    }

    context2D.save();
}

int main() {
    render();
    return 0;
}