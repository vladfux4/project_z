set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
enable_language(ASM)

set(CMAKE_C_COMPILER aarch64-elf-gcc)
set(CMAKE_CXX_COMPILER aarch64-elf-g++)
set(CMAKE_ASM_COMPILER aarch64-elf-as)
set(CMAKE_LINKER aarch64-elf-ld)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> -o <TARGET>")
set(CMAKE_OBJCOPY aarch64-elf-objcopy)

#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS "${PLATFORM_C_FLAGS} -g -Wall -nostdlib -ffreestanding -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${PLATFORM_CXX_FLAGS} -W -Wall -Wextra -g -std=c++17 -ffreestanding -nodefaultlibs -nostdlib -fno-exceptions -fno-rtti -fno-common -fno-builtin -fdata-sections -ffunction-sections")
SET(CMAKE_ASM_FLAGS "${PLATFORM_C_FLAGS} -g")
