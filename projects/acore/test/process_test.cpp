import atest;
import acore;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("acore::Process", [] {
    test("cmd", [] {
        acore::Process process{"cmd /c \"echo Hello, World!\""};
        std::cout << process.command() << '\n';
        std::cout << process.exitCode() << '\n';
    });
});