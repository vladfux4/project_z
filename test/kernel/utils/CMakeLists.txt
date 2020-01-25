set(CMAKE_CXX_STANDARD 17)

add_executable(utils_test
    register_test.cc
    variant_test.cc
    main.cc)

target_link_libraries(utils_test libgtest libgmock)
add_test(${PROJECT_NAME} utils_test)
