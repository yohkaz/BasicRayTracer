cmake_minimum_required(VERSION 3.10)
project(MyRayTracer)

find_package(OpenMP REQUIRED)

add_executable(MyRayTracer main.cpp Image.h Scene.h Model.h Camera.h Ray.h Vec3.h)
target_compile_options(MyRayTracer PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(MyRayTracer PRIVATE OpenMP::OpenMP_CXX)
