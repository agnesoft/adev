import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

class CoutRedirect
{
public:
    CoutRedirect()
    {
        std::cout.rdbuf(this->buf.rdbuf());
    }

    CoutRedirect(const CoutRedirect &other) = delete;
    CoutRedirect(CoutRedirect &&other) noexcept = delete;

    ~CoutRedirect()
    {
        try
        {
            std::cout.rdbuf(coutbuf);
        }
        catch (...)
        {
        }
    }

    [[nodiscard]] auto buffer() noexcept -> std::stringstream &
    {
        return this->buf;
    }

    auto operator=(const CoutRedirect &other) -> CoutRedirect & = delete;
    auto operator=(CoutRedirect &&other) noexcept -> CoutRedirect & = delete;

private:
    std::stringstream buf;
    std::streambuf *coutbuf = std::cout.rdbuf();
};

static const auto S = suite("throwing suite()", [] { // NOLINT(cert-err58-cpp)
    test("int", [] {
        expect([] {
            CoutRedirect redirect;
            ::atest::TestRunner runner;
            return suite("test suite", [] { throw 1; }); // NOLINT(hicpp-exception-baseclass)
        })
            .to_be(1);
    });

    test("int (text)", [] {
        expect([] {
            CoutRedirect redirect;
            ::atest::TestRunner runner{redirect.buffer()};
            suite("some suite", [] { throw 1; }); // NOLINT(hicpp-exception-baseclass)
            return redirect.buffer().str();
        })
            .to_be("ERROR: Unknown exception thrown during registration of 'some suite' test suite.\n");
    });

    test("logic_error", [] {
        expect([] {
            CoutRedirect redirect;
            ::atest::TestRunner runner;
            return suite("logic suite", [] { throw std::logic_error{"Bad test suite"}; });
        })
            .to_be(1);
    });

    test("logic_error (text)", [] {
        expect([] {
            CoutRedirect redirect;
            ::atest::TestRunner runner{redirect.buffer()};
            suite("logic test suite", [] { throw std::logic_error{"Really bad test suite"}; });
            return redirect.buffer().str();
        })
            .to_be("ERROR: Exception thrown during registration of 'logic test suite' test suite (" + std::string{typeid(std::logic_error).name()} + "): Really bad test suite\n");
    });
});
