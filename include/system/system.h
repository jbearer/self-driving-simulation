#pragma once

#include <atomic>
#include <functional>

#ifdef __arm__
#define R_PI
#endif

/**
 * Miscellaneous functions used throughout the project.
 */
namespace sys {
    /**
     * @brief      Do some work if this is the first instantiation of a type.
     * @details    For any type T:
     *             The first time if_first<T> is called, the given function is executed.
     *             Subsequent calls to if_first<T> have no effect.
     */
    template<typename any_t>
    void if_first(std::function<void(void)> const & func, any_t *** = 0)
    {
        static std::atomic<bool> token(true);
        bool first = std::atomic_exchange(&token, false);
        if (first) {
            func();
        }
    }
}
