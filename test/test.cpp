#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "diagnostics/diag.h"
#include "objects/objects.h"
#include "testing.h"

using namespace std;
using namespace testing;

static diagnostics::logger & diag() {
    static diagnostics::logger * diag_ = new diagnostics::logger("testing");
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

    void erase(string const & key)
    {
        if ( key.empty() ) {
            // Matches everything from here down
            matching_test = false;
            children.clear();
        } else {
            auto tokens = extract_prefix(key);
            if (children[tokens.first]) {
                children[tokens.first]->erase(tokens.second);
            }
        }
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

static test_trie & tests()
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

static int run_tests(vector<string> const & keys)
{
    int num_tests = 0;

    for (auto const & key : keys) {
        auto test_cases = tests().find(key);

        for (auto const & test : test_cases) {
            diag().info("Begin {}.", test.name);
            test.run();
            diag().info("Passed {}.", test.name);
            objects::reset();
            ++num_tests;
        }
    }

    return num_tests;
}

int main(int argc, char ** argv)
{
    int num_tests = 0;

    vector<string>  tests_to_exclude;
    vector<string>  tests_to_run;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '^') {
            tests_to_exclude.push_back(argv[i] + 1);
        } else {
            tests_to_run.push_back(argv[i]);
        }
    }

    // Before we start erasing the excluded tests, we make sure everything that the user typed in is
    // actually a registered test. This involves doing a bit of extra work, but this isn't really
    // performance sensitive and it let's us give nice error messages.
    for (auto const & key : tests_to_run) {
        diag_assert(!tests().find(key).empty(), "No test case matching {}.", key);
    }

    // Now remove excluded tests from the trie.
    for (auto const & key : tests_to_exclude) {
        diag_assert(!tests().find(key).empty(), "No test case matching {}.", key);
        tests().erase(key);
    }

    if ( tests_to_run.empty() ) {
        // An empty string matches every test
        tests_to_run.push_back("");
    }

    num_tests = run_tests(tests_to_run);

    diag_assert(num_tests, "No test cases found.");
    diag().info("Passed {} tests.", num_tests);

    return 0;
}
