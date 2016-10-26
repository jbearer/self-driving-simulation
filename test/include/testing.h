#pragma once

#include <functional>

namespace testing {

    typedef std::function<void()> test_func_t;

    struct test_case_registration
    {
        test_case_registration(char const * name, test_func_t const & test);
    };
}

#define __testing_concat(x, y)              x ## y
#define __testing_id_with_count(count)      __testing_concat(test_registration, count)
#define __testing_unique_id                 __testing_id_with_count(__COUNTER__)

#define __testing_test_case_with_func(name, id, func) \
    static void func (); \
    static testing::test_case_registration id (#name, &func); \
    void func ()

#define __testing_test_case(name, unique_id) \
    __testing_test_case_with_func( name, unique_id, __testing_concat(unique_id, _func) )

#define test_case(name) __testing_test_case(name, __testing_unique_id)
