import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("input", [] { // NOLINT(cert-err58-cpp)
    test("empty input", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-input")
                                          .read([&](std::string_view message) { output += message; })
                                          .write();

        process.write("\n");
        process.write("exit\n");
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});

        expect(output).to_be(std::string{});
    });

    test("Hello, World!", [] {
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
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});

        expect(output).to_be(std::string{"Hello,World!"});
    });

    test("large input", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-input")
                                          .read([&](std::string_view message) { output += message; })
                                          .write();

        constexpr std::size_t LARGE_SIZE = 1'000'000;
        std::string input(LARGE_SIZE, 'A');

        process.write(input);
        process.write("\n");
        process.write("exit\n");
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});

        expect(output.size()).to_be(LARGE_SIZE);
        expect(output == input).to_be(true);
    });

    test("write when input is disabled", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo-delay", "10", "--echo", "\"Hello\"", "--echo", "\"World\"", "--echo", "\"!\""});

        expect([&] {
            process.write("message");
        })
            .to_throw<std::logic_error>("The process does not have stdin pipe open.");
    });

    test("write to stopped process", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .read([&](std::string_view message) { output += message; })
                                          .write()
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        process.write("message\n");

        expect(output).to_be(std::string{});
    });
});
