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

    ~CoutRedirect()
    {
        std::cout.rdbuf(coutbuf);
    }

    [[nodiscard]] auto buffer() noexcept -> std::stringstream &
    {
        return this->buf;
    }

private:
    std::stringstream buf;
    std::streambuf *coutbuf = std::cout.rdbuf();
};

static const auto S = suite("throwing suite()", [] { // NOLINT(cert-err58-cpp)
    test("int", [] {
        expect([] {
            CoutRedirect redirect;
            ::atest::TestRunner runner;
            return suite("test suite", [] { throw 1; });
        })
            .to_be(1);
    });

    test("int (text)", [] {
        expect([] {
            CoutRedirect redirect;
            ::atest::TestRunner runner{redirect.buffer()};
            suite("some suite", [] { throw 1; });
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
