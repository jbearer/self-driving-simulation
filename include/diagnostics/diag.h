#pragma once

#include "diagnostics/logging.h"

namespace diagnostics {

    namespace diagnostics_impl {
        static logger diag("diagnostics");

        /**
         * @brief      Handler for plain asserts. Should be called via the macro assert.
         *
         * @param      cond  The condition which caused the assertion to fail.
         * @param      file  The file containing the assertion.
         * @param[in]  line  The line number at which the assertion ocurred.
         */
        inline void assert_failed(char const * cond, char const * file, int line)
        {
            diag.fail("Assertion failed ({}:{}): assert({})", file, line, cond);
        }

        /**
         * @brief      Handler for asserts with a custom log message.
         *
         * @param      msg   The message format string.
         * @param[in]  args  The arguments to be passed to the formatter
         */
        template<typename... args_t>
        inline void assert_failed(char const * cond,
                           char const * file,
                           int line,
                           std::string const & msg,
                           const args_t&... args)
        {
            diag.fail("Assertion failed ({}:{}): {}", file, line, msg, args...);
        }
    }
}

#define __diag_assert_stringize(x) #x

#define diag_assert(cond, ...) do { \
    if ( !(cond) ) { \
        diagnostics::diagnostics_impl::assert_failed( \
            __diag_assert_stringize(cond), __FILE__, __LINE__, ##__VA_ARGS__); \
    } \
} while(0)
