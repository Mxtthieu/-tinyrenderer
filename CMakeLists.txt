cmake_minimum_required(VERSION 3.15)
project(tinyrenderer)

set(CMAKE_CXX_STANDARD 14)

add_executable(tinyrenderer src/main.cpp src/math/Vector.h src/2DGraphic/Graphic2D.cpp src/2DGraphic/Graphic2D.h src/ModelLoader.cpp src/ModelLoader.h  src/tgaimage.cpp src/tgaimage.h src/math/Matrix.h)