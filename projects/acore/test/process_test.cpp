import atest;
import acore;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("acore::Process", [] {
    test("cmd", [] {
        acore::Process process{"cmd /c \"echo Hello, World! && echo error 1>&2\""};
        std::cout << process.command() << '\n';
        std::cout << process.exitCode() << '\n';
        std::cout << process.standardOutput() << '\n';
        std::cout << process.standardError() << '\n';
    });
});