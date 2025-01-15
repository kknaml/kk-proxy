
# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/botan/build/cmake")

# find_package(Botan REQUIRED)

add_subdirectory(spdlog)
add_subdirectory(asio)
add_subdirectory(thread-pool)