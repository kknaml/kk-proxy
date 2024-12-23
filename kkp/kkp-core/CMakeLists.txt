
file(GLOB_RECURSE KKP_SOURCES src/*.cpp test/*.c)

add_library(kkp-core STATIC ${KKP_SOURCES})

target_include_directories(kkp-core PUBLIC include)


