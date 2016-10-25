#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "logging/logging.h"
#include "testing/testing.h"

using namespace std;
using namespace testing;

logging::logger & diag() {
    static logging::logger * diag_ = new logging::logger("testing");
    return *diag_;
}

struct test_functor
{
    string      name;
    test_func_t run;

    test_functor() = default;

    test_functor(const test_functor &) = default;

    test_functor(string const & name_, test_func_t const & test_)
        : name(name_)
        , run(test_)
    {}
};

struct test_trie
{
    enum insert_status
    {
        DUPLICATE,
        SUCCESS
    };

    test_trie()
        : matching_test(false)
    {}

    insert_status insert(string const & key, test_functor const & val)
    {
        if ( key.empty() ) {
            if (matching_test) {
                return DUPLICATE;
            } else {
                matching_test = true;
                test = val;
                return SUCCESS;
            }
        } else {
            auto tokens = test_trie::extract_prefix(key);
            if (!children[tokens.first]) {
                children[tokens.first] = make_unique<test_trie>();
            }
            return children[tokens.first]->insert(tokens.second, val);
        }
    }

    vector<test_functor> find(string const & key)
    {
        if ( key.empty() ) {
            return all();
        } else {
            auto tokens = extract_prefix(key);
            if (children[tokens.first]) {
                return children[tokens.first]->find(tokens.second);
            } else {
                return vector<test_functor>();
            }
        }
    }

    vector<test_functor> all() const
    {
        vector<test_functor> results;
        if (matching_test) {
            results.push_back(test);
        }
        for (auto const & child : children) {
            if (child.second) {
                auto all_children = child.second->all();
                results.insert(results.end(), all_children.begin(), all_children.end());
            }
        }
        return results;
    }

private:
    static pair<string, string> extract_prefix(string const & key)
    {
        stringstream ss(key);
        pair<string, string> output;
        getline(ss, output.first, '.');
        ss >> output.second;
        return output;
    }

    bool                                            matching_test;
    test_functor                                    test;
    unordered_map<string, unique_ptr<test_trie> >   children;
};

test_trie & tests()
{
    static test_trie * test_ = new test_trie;
    return *test_;
}

testing::test_case_registration::test_case_registration(char const * name, test_func_t const & test)
{
    switch ( tests().insert( string(name), test_functor(string(name), test) ) ) {
    case test_trie::DUPLICATE:
        diag().fail("Duplicate test case {}.", name); break;
    case test_trie::SUCCESS:
        break;
    }
}

int run_tests(string const & key)
{
    auto test_cases = tests().find(key);

    if ( test_cases.empty() ) {
        diag().fail("No test cases matching {}.", key);
    }

    for (auto const & test : test_cases) {
        diag().info("Begin {}.", test.name);
        test.run();
        diag().info("Passed {}.", test.name);
    }

    return test_cases.size();
}

int run_all_tests()
{
    return run_tests("");
}

int main(int argc, char ** argv)
{
    int num_tests = 0;
    if (argc == 1) {
        num_tests = run_all_tests();
    } else {
        for (int i = 1; i < argc; ++i) {
            num_tests += run_tests( string(argv[i]) );
        }
    }

    diag().info("Passed {} tests.", num_tests);

    return 0;
}
