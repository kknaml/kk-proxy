
add_library(asio INTERFACE)

target_include_directories(asio INTERFACE include)
target_link_libraries(asio INTERFACE OpenSSL::SSL)