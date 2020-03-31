I worked on a Windows system with the compiler MinGW, but the code should also work on Linux.

Instructions to compile the code in Windows:
- mkdir build
- cd build
- cmake.exe -G "MinGW Makefiles" ../
- mingw32-make.exe

Then run the code:
- ./BasicRayTracer.exe

Instructions to compile the code in Linux:
- mkdir build
- cd build
- cmake ../
- make

Then run the code:
- ./BasicRayTracer


The main files about the paper (https://arxiv.org/abs/1701.07403) method implementation are:
- experiments.cpp (defines experimented scenes)
- Qtable.h
- HemisphereMapping.h
