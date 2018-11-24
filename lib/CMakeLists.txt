find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
  gtest
  URL https://github.com/google/googletest/archive/master.zip
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
set_target_properties(libgtest PROPERTIES
  IMPORTED_LOCATION "${binary_dir}/lib/libgtest.a"
  IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
set_target_properties(libgmock PROPERTIES
  IMPORTED_LOCATION "${binary_dir}/lib/libgmock.a"
  IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
)

set(GTEST_INCLUDE_DIRS 
  "${source_dir}/googletest/include"
  "${source_dir}/googlemock/include" 
  PARENT_SCOPE
)

set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
