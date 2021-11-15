import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{100};

static const auto S = suite("output", [] { // NOLINT(cert-err58-cpp)
    test("no output", [] {
        std::string output;
        ::aprocess::Process process{{.command = "aprocesstestapp",
                                     .read = [&](std::string_view message, [[maybe_unused]] ::aprocess::Process &process) { output += message; }}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(output).to_be(std::string{});
    });

    test("Hello, World!", [] {
        std::string output;
        ::aprocess::Process process{{.command = "aprocesstestapp",
                                     .arguments = {"--echo=Hello", "--echo=,", "--echo=World", "--echo=!"},
                                     .read = [&](std::string_view message, [[maybe_unused]] ::aprocess::Process &process) { output += message; }}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(output).to_be(std::string{"Hello,World!"});
    });

    test("large output", [] {
        std::string output;
        ::aprocess::Process process{{.command = "aprocesstestapp",
                                     .arguments = {"--echo-large=A", "--echo-large-size=1000000"},
                                     .read = [&](std::string_view message, [[maybe_unused]] ::aprocess::Process &process) { output += message; }}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        constexpr std::size_t LARGE_SIZE = 1'000'000;
        assert_(output.size()).to_be(LARGE_SIZE);
        expect(output == std::string(LARGE_SIZE, 'A')).to_be(true);
    });
});
