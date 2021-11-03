import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("synchronous process", [] {
        // clang-format off
//! [[synchronous process]]
std::string output;
::aprocess::Process process{{.command = "aprocesstestapp",
                             .arguments = {"--echo", "\"Hello, World!\""},
                             .read = [&](std::string_view message, [[maybe_unused]] ::aprocess::Process &process) { output += message; } }};
process.wait(std::chrono::milliseconds{1000});
//! [[synchronous process]]
        // clang-format on

        expect(output).to_be("Hello, World!");
    });

    test("real time output", [] {
        // clang-format off
//! [[real time output]]
std::vector<std::string> output;
::aprocess::Process process{{.command = "aprocesstestapp",
                             .arguments = {"--echo-delay", "10", "--echo", "\"Hello\"", "--echo", "\"World\"", "--echo", "\"!\""},
                             .read = [&](std::string_view message, [[maybe_unused]] ::aprocess::Process &process) { output.emplace_back(std::string{message}); } }};
process.wait(std::chrono::milliseconds{1000});
//! [[real time output]]
        // clang-format on

        expect(output).to_be(std::vector<std::string>{"Hello", "World", "!"});
    });

    test("input", [] {
        // clang-format off
//! [[input]]
std::vector<std::string> input{"exit", "!", "World"};
std::vector<std::string> output;
::aprocess::Process process{{.command = "aprocesstestapp",
                             .arguments = {"--echo-input", "--echo=Hello"},
                             .read = [&](std::string_view message, [[maybe_unused]] ::aprocess::Process &process) {
                                 output.emplace_back(std::string{message});

                                 if (!input.empty())
                                 {
                                     process.write(input.back());
                                     input.pop_back();
                                 }
                             },
                             .write = true }};

process.wait(std::chrono::milliseconds{1000});
//! [[input]]
        // clang-format on

        expect(output).to_be(std::vector<std::string>{"Hello", "World", "!"});
    });
});
