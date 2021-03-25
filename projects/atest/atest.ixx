export module atest;

import <vector>;

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

export auto test(const char *description, auto (*testBody)()->void) -> void
{
    TEST_SUITES.front().tests.push_back(Test{description, testBody});
}
}
