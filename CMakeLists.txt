project (project_z)
cmake_minimum_required(VERSION 2.8)

set(CPU_ARCH arm64)
set(PLATFORM_C_FLAGS "-mcpu=cortex-a53")

add_subdirectory(lib)
add_subdirectory(src)
enable_testing()
add_subdirectory(test)
