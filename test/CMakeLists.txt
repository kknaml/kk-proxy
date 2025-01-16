file(GLOB TEST_SOURCES "*.cpp")

foreach (TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    set(TEST_NAME "test-${TEST_NAME}")
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PUBLIC kkp-core kkp-http uring asio)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach ()