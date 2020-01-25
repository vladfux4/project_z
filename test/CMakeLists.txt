include_directories(
  "../src"
  ${GTEST_INCLUDE_DIRS}
)

add_subdirectory(kernel/scheduler)
add_subdirectory(kernel/mm)
add_subdirectory(kernel/utils)
