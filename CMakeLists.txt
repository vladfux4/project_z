project (project_z)
cmake_minimum_required(VERSION 2.8)

set (CPU_ARCH arm64)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
configure_file(src/arch/arch_types_gen.in include/gen/arch_types_gen.h)

add_subdirectory(lib)
add_subdirectory(src/arch/${CPU_ARCH})
add_subdirectory(src/kernel)

enable_testing()
add_subdirectory(test)
