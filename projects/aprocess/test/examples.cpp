import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("synchronous process", [] {
        // clang-format off
//! [[synchronous process]]
aprocess::Process process{{.command = "echo Hello World"}};
process.wait();
std::cout << process.read();
//! [[synchronous process]]
        // clang-format on
    });

    test("real time output", [] {
        // clang-format off
//! [[real time output]]
aprocess::Process process{{.command = "echo Hello World"}};
while (process.is_running()) //beware of possible deadlock, consider offloading this to a new thread
{
    std::cout << process.read();
}
std::cout << process.read(); //make sure you get the rest of the output after the process stopped
//! [[real time output]]
        // clang-format on
    });

    test("input", [] {
        // clang-format off
//! [[input]]
aprocess::Process process{{.command = "echo Hello && read name && echo $name"}};
process.write("AProcess");
process.wait();
//! [[input]]
        // clang-format on
    });
});