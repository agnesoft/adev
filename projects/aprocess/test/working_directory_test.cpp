import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("working directory", [] { // NOLINT(cert-err58-cpp)
    test("default", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-working-directory")
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.working_directory()).to_be(std::filesystem::current_path().string());
        expect(output).to_be(std::filesystem::current_path().string());
    });

    test("custom", [] {
        std::string output;
        const std::filesystem::path dir = std::filesystem::canonical(std::filesystem::current_path().root_directory());

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-working-directory")
                                          .working_directory(dir.string())
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.working_directory()).to_be(dir.string());
        expect(output).to_be(dir.string());
    });
});
