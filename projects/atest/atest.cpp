module;

#ifdef _MSC_VER
#    include <typeinfo>
#endif

export module atest;

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 5106)
#endif

export import "astl.hpp";

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

export import : expect;
export import : test_runner;

namespace atest
{
export template<typename T>
[[nodiscard]] auto expect(const T &value, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> Expect<T>
{
    return Expect<T>{value, sourceLocation};
}

export template<typename T = int>
auto test(const char *name, auto (*testBody)()->void, const source_location<> &sourceLocation = source_location<>::current()) -> void
{
    globalTests()->currentTestSuite->tests.emplace_back(Test{name, testBody, sourceLocation});
}

export template<typename T = int>
auto suite(const char *name, auto (*suiteBody)()->void, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> int
{
#ifdef _MSC_VER
    using ::type_info;
#endif

    try
    {
        try
        {
            Tests *tests = globalTests();
            tests->currentTestSuite = &tests->suites.emplace_back(TestSuite{name, sourceLocation});
            suiteBody();
            tests->currentTestSuite = &tests->suites.front();
            return 0;
        }
        catch (std::exception &e)
        {
            std::cout << "ERROR: Exception thrown during registration of '" << name << "' test suite (" << typeid(e).name() << "): " << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "ERROR: Unknown exception thrown during registration of '" << name << "' test suite.\n";
        }
    }
    catch (...)
    {
        //Supress any further exceptions as this function is
        //usually run outside of main and no exception can
        //be caught. See clang-tidy check: cert-err58-cpp.
    }

    return 1;
}
}
