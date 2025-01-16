cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
#set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD 23)
project(kkproxy LANGUAGES CXX C)

# set(CMAKE_CXX_COMPILER_IMPORT_STD ON)



find_package(OpenSSL REQUIRED)


# target_link_options(main PRIVATE -stdlib=libc++)
add_subdirectory(third_party)
#add_subdirectory(app)
add_subdirectory(kkp)
add_subdirectory(test)