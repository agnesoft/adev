import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("synchronous process", [] {
        // clang-format off
//! [[synchronous process]]
::aprocess::Process process{{.command = "aprocesstestapp",
                             .arguments = {"--echo", "\"Hello, World!\""}}};
process.wait(std::chrono::milliseconds{1000});
const std::string output = process.read();
//! [[synchronous process]]
        // clang-format on

        expect(output).to_be("Hello, World!");
    });

    test("real time output", [] {
        // clang-format off
//! [[real time output]]
::aprocess::Process process{{.command = "aprocesstestapp",
                             .arguments = {"--echo-delay", "50", "--echo", "\"Hello\"", "--echo", "\"World\"", "--echo", "\"!\""}}};
std::stringstream out;

while (process.is_running())
{
    out << process.read();
}
//! [[real time output]]
        // clang-format on

        expect(out.str()).to_be(std::string{"HelloWorld!"});
    });

    test("input", [] {
        // clang-format off
//! [[input]]
::aprocess::Process process{{.command = "aprocesstestapp",
                             .arguments = {"--echo-input"}}};
std::vector<std::string> output;
process.write("Hello");
output.push_back(process.read(std::chrono::milliseconds{100}));
process.write("World");
output.push_back(process.read(std::chrono::milliseconds{100}));
process.write("!");
output.push_back(process.read(std::chrono::milliseconds{100}));
process.write("exit");
process.wait(std::chrono::milliseconds{1000});
//! [[input]]
        // clang-format on

        expect(output).to_be(std::vector<std::string>{"Hello", "World", "!"});
    });
});
