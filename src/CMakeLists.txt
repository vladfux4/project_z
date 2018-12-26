include(arch/${CPU_ARCH}/cmake/compiler.cmake)
configure_file(arch/arch_types_gen.in include/gen/arch_types_gen.h)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
set(SOURCE "" CACHE INTERNAL "" FORCE)

set(LD_FILE ${CMAKE_CURRENT_SOURCE_DIR}/kernel/kernel.ld)
set(CMAKE_EXE_LINKER_FLAGS "-T ${LD_FILE} -gc-sections")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")

add_subdirectory(arch/${CPU_ARCH})
add_subdirectory(kernel)

set(COMPONENT_NAME kernel_image)
add_executable(${COMPONENT_NAME} ${SOURCE})
add_custom_command(TARGET ${COMPONENT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT_NAME}.bin
)
add_custom_command(TARGET ${COMPONENT_NAME}
  POST_BUILD
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/../../bin/${COMPONENT_NAME}.elf
)
add_custom_command(TARGET ${COMPONENT_NAME}
  POST_BUILD
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${COMPONENT_NAME}.bin ${CMAKE_CURRENT_BINARY_DIR}/../../bin/${COMPONENT_NAME}.bin
)
