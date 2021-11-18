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
::aprocess::create_process()
    .command("aprocesstestapp")
    .arg("--echo")
    .arg("\"Hello, World!\"")
    .read([&](std::string_view message) { output += message; })
    .wait(std::chrono::seconds{1});
//output == "Hello, World!"
//! [[synchronous process]]
        // clang-format on

        expect(output).to_be("Hello, World!");
    });

    test("real time output", [] {
        // clang-format off
//! [[real time output]]
std::vector<std::string> output;
::aprocess::create_process()
    .command("aprocesstestapp")
    .arguments({"--echo-delay", "10", "--echo", "\"Hello\"", "--echo", "\"World\"", "--echo", "\"!\""})
    .read([&](std::string_view message) { output.emplace_back(std::string{message}); })
    .wait(std::chrono::seconds{1});
//output == {"Hello", "World", "!"}
//! [[real time output]]
        // clang-format on

        expect(output).to_be(std::vector<std::string>{"Hello", "World", "!"});
    });

    test("input", [] {
        // clang-format off
        //! [[input]]
std::string output;

::aprocess::Process process = ::aprocess::create_process()
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
