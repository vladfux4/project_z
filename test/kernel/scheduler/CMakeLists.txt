add_executable(scheduler
    routine_static_wrapper_test.cc
    main.cc)

target_link_libraries(scheduler ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} pthread)
add_test(${PROJECT_NAME} scheduler)
