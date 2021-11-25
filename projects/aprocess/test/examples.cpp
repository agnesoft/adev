import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("process", [] {
        // clang-format off
//! [[process]]
std::string output;
::aprocess::create_process()
    .command("aprocesstestapp")
    .arg("--echo")
    .arg("\"Hello, World!\"")
    .read([&](std::string_view message) { output += message; })
    .wait(std::chrono::seconds{1});
//output == "Hello, World!"
//! [[process]]
        // clang-format on

        expect(output).to_be("Hello, World!");
    });

    test("input", [] {
        // clang-format off
        //! [[input]]
std::string output;

::aprocess::Process process = 
    ::aprocess::create_process()
        .command("aprocesstestapp")
        .arg("--echo-input")
        .read([&](std::string_view message) { output += message; })
        .write();

process.write("Hello\n");
process.write(",\n");
process.write("World\n");
process.write("!\n");
process.write("exit\n");

process.wait(std::chrono::seconds{1});
// output == "Hello, World!"
        //! [[input]]
        // clang-format on

        expect(output).to_be("Hello,World!");
    });
});
