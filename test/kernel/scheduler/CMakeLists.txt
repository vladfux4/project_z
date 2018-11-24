add_executable(scheduler_test
    routine_static_wrapper_test.cc
    main.cc)

target_link_libraries(scheduler_test libgtest libgmock)
add_test(${PROJECT_NAME} scheduler_test)
