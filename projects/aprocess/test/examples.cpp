import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("process", [] {
        // clang-format off
//! [[process]]
std::string output = 
    ::aprocess::create_process()
        .command("aprocesstestapp")
        .arg("--echo")
        .arg("\"Hello, World!\"")
        .wait(std::chrono::seconds{1})
        .read();
//output == "Hello, World!"
//! [[process]]
        // clang-format on

        expect(output).to_be("Hello, World!");
    });

    test("input", [] {
        // clang-format off
//! [[input]]
::aprocess::Process process = 
    ::aprocess::create_process()
        .command("aprocesstestapp")
        .arg("--echo-input");

process.write("Hello\n");
process.write(",\n");
process.write("World\n");
process.write("!\n");
process.write("exit\n");

process.wait(std::chrono::seconds{2});
std::string output = process.read();
// output == "Hello, World!"
//! [[input]]
        // clang-format on

        expect(output).to_be("Hello,World!");
    });

    test("output", [] {
        // clang-format off
//! [[output]]
::aprocess::Process process = 
    ::aprocess::create_process()
        .command("aprocesstestapp")
        .arg("--echo=Hello")
        .arg("--echo=,")
        .arg("--echo=World")
        .arg("--echo=!")
        .arg("--echo-delay=10");

std::string output;

while (process.is_running())
{
    output += process.read(std::chrono::milliseconds{1});
}
// output == "Hello, World!"
//! [[output]]
        // clang-format on

        expect(output).to_be("Hello,World!");
    });
});
