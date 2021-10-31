import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("0", [] {
        aprocess::Process process{{.command = "aprocesstestapp"}};
        process.wait();
        expect(process.exit_code()).to_be(0);
    });

    test("1", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments{"--exit-code", "1"}}};
        process.wait();
        expect(process.exit_code()).to_be(1);
    });
});
