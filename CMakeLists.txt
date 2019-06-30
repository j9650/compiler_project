cmake_minimum_required(VERSION 3.10)
project(Fluid)
set(CMAKE_C_COMPILER "/home/mdl/hzj5142/soft/install4/gcc-7.0.1/bin/gcc")
set(CMAKE_CXX_COMPILER "/home/mdl/hzj5142/soft/install4/gcc-7.0.1/bin/g++")
#set(CMAKE_C_COMPILER "/home/mdl/hzj5142/soft/install2/bin/gcc")
#set(CMAKE_CXX_COMPILER "/home/mdl/hzj5142/soft/install2/bin/g++")
#set(CMAKE_C_COMPILER "/home/mdl/huz123/software/llvm-build/bin/clang")
#set(CMAKE_CXX_COMPILER "/home/mdl/huz123/software/llvm-build/bin/clang++")


set(CMAKE_CXX_STANDARD 17)
set(FLUID_FILES
        FluidProject/fluid/fluid.h FluidProject/fluid/guard.cpp FluidProject/fluid/guard.h
        FluidProject/fluid/guardscheduler.cpp FluidProject/fluid/guardscheduler.h
        FluidProject/fluid/guardstates.cpp FluidProject/fluid/guardstates.h
        FluidProject/fluid/signal.h
        FluidProject/fluid/syncds.h
        FluidProject/fluid/thread.h
        FluidProject/fluid/tasklet.h FluidProject/fluid/tasklet.cpp
        FluidProject/fluid/threadpool.h
        FluidProject/fluid/data.h FluidProject/fluid/data.cpp
        FluidProject/fluid/valve.h FluidProject/fluid/tasklet.cpp)

set(INPUT_FILES
        FluidProject/apps/experiments.cpp FluidProject/apps/experiments.h
        FluidProject/apps/kmeans.cpp FluidProject/apps/kmeans.h FluidProject/apps/kmeans_fluid.cpp FluidProject/apps/kmeans_fluid.h
        FluidProject/apps/matrix.cpp FluidProject/apps/matrix.h FluidProject/apps/matrix_fluid.h 
	FluidProject/apps/bellman_ford.cpp FluidProject/apps/bellman_ford.h FluidProject/apps/bellman_ford_fluid.cpp FluidProject/apps/bellman_ford_fluid.h
	FluidProject/apps/graphcolor.cpp FluidProject/apps/graphcolor.h FluidProject/apps/graphcolor_fluid.cpp FluidProject/apps/graphcolor_fluid.h
	FluidProject/apps/sobel.cpp FluidProject/apps/sobel.h FluidProject/apps/sobel_fluid.cpp FluidProject/apps/sobel_fluid.h
        FluidProject/apps/rgb_image.h
        FluidProject/apps/test_out.cpp FluidProject/apps/test_out.h
        FluidProject/apps/sssp.cpp FluidProject/apps/sssp.h FluidProject/apps/sssp_fluid.cpp FluidProject/apps/sssp_fluid.h
        )

set(UTILS_FILES
        FluidProject/utils/logger.cpp FluidProject/utils/logger.h
        FluidProject/utils/profiler.cpp FluidProject/utils/profiler.h
        FluidProject/utils/timer.cpp FluidProject/utils/timer.h
        )
set (CMAKE_CXX_FLAGS "-Wall -g -O2 -ggdb --std=c++17 -std=gnu++17 -pthread")
add_executable(fluid FluidProject/main.cpp ${FLUID_FILES} ${INPUT_FILES} ${UTILS_FILES})
