export module atest;

import<iostream>;
import<vector>;

namespace atest
{
struct Test
{
    const char *description = "";
    auto (*testBody)() -> void = nullptr;
};

struct TestSuite
{
    const char *name = "";
    std::vector<Test> tests;
};

std::vector<TestSuite> TEST_SUITES = std::vector<TestSuite>{{"Global", {}}};
TestSuite &TEST_SUITE = TEST_SUITES.front();

export auto test(const char *description, auto (*testBody)()->void) -> void
{
    std::cout << "  Test '" << description << "'\n";
    TEST_SUITE.tests.push_back(Test{description, testBody});
}

export auto suite(const char *name, auto (*suiteBody)()->void) -> int
{
    std::cout << "Suite '" << name << "'\n";
    TEST_SUITES.push_back({name, {}});
    TEST_SUITE = TEST_SUITES.back();
    suiteBody();
    TEST_SUITE = TEST_SUITES.front();
    return 0;
}
}
