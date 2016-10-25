#pragma once

#include <functional>

namespace testing {

    typedef std::function<void()> test_func_t;

    struct test_case_registration
    {
        test_case_registration(char const * name, test_func_t const & test);
    };
}

#define __testing_expand(count)           test_registration_ ## count
#define __testing_name_with_count(count)  __testing_expand(count)
#define __testing_unique_name             __testing_name_with_count(__COUNTER__)

#define test_case(name, func) \
    static test_case_registration __testing_unique_name (#name, func)
