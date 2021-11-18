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

        assert_(process.writable()).to_be(true);
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

        assert_(process.writable()).to_be(true);
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

        constexpr std::size_t largeSize = 1'000'000;
        std::string input(largeSize, 'A');

        assert_(process.writable()).to_be(true);
        process.write(input);
        process.write("\n");
        process.write("exit\n");
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});

        expect(output.size()).to_be(largeSize);
        expect(output == input).to_be(true);
    });

    test("write when input is disabled", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo-delay", "10", "--echo", "\"Hello\"", "--echo", "\"World\"", "--echo", "\"!\""});

        assert_(process.writable()).to_be(false);
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

        assert_(process.writable()).to_be(false);
        expect(output).to_be(std::string{});
    });
});
