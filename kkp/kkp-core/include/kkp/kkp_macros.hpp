#pragma once

#include <print>

#define CONCATENATE(x, y) x##y

#ifdef NDEBUG
#define KKP_DEBUG_ASSERT(X)
#else
#define KKP_DEBUG_ASSERT(X) if (!(X)) { \
    std::println("Assertion failed: {} in {} at line {}", #X, __FILE__, __LINE__);\
    std::abort(); \
}
#endif

