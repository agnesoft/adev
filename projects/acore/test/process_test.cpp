import atest;
import acore;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("acore::Process", [] {
    test("cmd", [] {
        acore::Process process{"cmd /c \"echo Hello, World!\""};
        std::cout << "Command  : " << process.command() << '\n';
        std::cout << "Exit code: " << process.exitCode() << '\n';
        std::cout << "Output   : " << process.output() << '\n';
    });
});