add_executable(mm_test
    pool_test.cc
    main.cc)

target_link_libraries(mm_test libgtest libgmock)
add_test(${PROJECT_NAME} mm_test)
