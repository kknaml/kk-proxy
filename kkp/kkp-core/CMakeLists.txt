
file(GLOB_RECURSE KKP_SOURCES src/*.cpp test/*.c src/**.hpp)

add_library(kkp-core STATIC ${KKP_SOURCES})

target_include_directories(kkp-core PUBLIC include)

target_link_libraries(kkp-core PUBLIC dp::thread-pool uring spdlog)


