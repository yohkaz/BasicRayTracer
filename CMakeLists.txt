cmake_minimum_required(VERSION 3.10)
project(BasicRayTracer)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP REQUIRED)

add_executable(BasicRayTracer main.cpp
                              src/Image.h
                              src/Scene.h
                              src/Model.h
                              src/Camera.h
                              src/Ray.h
                              src/Vec3.h
                              src/Material.h
                              src/RayTracer.h
                              src/Light.h
                              src/PointLight.h
                           )
target_compile_options(BasicRayTracer PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(BasicRayTracer PRIVATE OpenMP::OpenMP_CXX)
