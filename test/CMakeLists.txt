set (GTEST_ROOT /usr/src/gtest)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(GMOCK_ROOT /usr/src/gmock)
set(GMOCK_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}/include)
set(GMOCK_LIBRARIES ${GMOCK_ROOT}/libgmock.a)

include_directories(${GMOCK_INCLUDE_DIRS})

include_directories("../src")

add_subdirectory(kernel/scheduler)
