



file(GLOB_RECURSE KKP_SOURCES src/*.cpp test/*.c)
file(GLOB_RECURSE KKP_MODULES src/*.ixx)

add_executable(kkproxy main.cpp ${KKP_SOURCES})
target_include_directories(kkproxy PUBLIC include)
target_link_libraries(kkproxy PRIVATE asio)

target_compile_features(kkproxy PRIVATE cxx_std_23)


# test
#file(GLOB_RECURSE KKP_TEST_SOURCES test/*.cpp)
#
#foreach (test_source ${KKP_TEST_SOURCES})
#    get_filename_component(test_name ${test_source} NAME_WE)
#    add_executable(${test_name} ${test_source} ${KKP_SOURCES})
#    # target_link_libraries(${test_name} PRIVATE asio)
#    target_include_directories(${test_name} PUBLIC include)
#    target_compile_features(${test_name} PUBLIC cxx_std_23)
#endforeach ()

