
file(GLOB_RECURSE KKP_SOURCES src/*.cpp test/*.c src/**.hpp)

add_library(kkp-http STATIC ${KKP_SOURCES})

target_include_directories(kkp-http PUBLIC include)

target_link_libraries(kkp-http PUBLIC kkp-core dp::thread-pool uring spdlog)


