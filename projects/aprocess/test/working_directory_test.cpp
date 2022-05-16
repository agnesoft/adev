import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("working directory", [] { // NOLINT(cert-err58-cpp)
    test("default", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo-working-directory")
                .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.working_directory()).to_be(std::filesystem::current_path());
        expect(std::filesystem::path{process.read()}).to_be(std::filesystem::current_path());
    });

    test("custom", [] {
        const std::filesystem::path dir = std::filesystem::current_path().parent_path();

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-working-directory")
                                          .working_directory(dir)
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.working_directory()).to_be(dir);
        expect(process.read()).to_contain(dir.string());
    });
});
