cmake_minimum_required(VERSION 3.10)
project(BasicRayTracer)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP REQUIRED)

file(GLOB SRC "*.h" "*.cpp")
add_executable(BasicRayTracer ${SRC})
target_compile_options(BasicRayTracer PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(BasicRayTracer PRIVATE OpenMP::OpenMP_CXX)
