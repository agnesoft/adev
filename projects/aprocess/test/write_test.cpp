import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{5000};

static const auto S = suite("write", [] { // NOLINT(cert-err58-cpp)
    test("empty input", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo-input");

        process.write("\n");
        process.write("exit\n");
        process.wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.read()).to_be(std::string{});
    });

    test("Hello, World!", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo-input");

        process.write("Hello\n");
        process.write(",\n");
        process.write("World\n");
        process.write("!\n");
        process.write("exit\n");
        process.wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.read()).to_be("Hello,World!");
    });

    test("large input", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo-input")
                .arg("--echo-input-timeout=5000");

        constexpr std::size_t largeSize = 1'000'000;
        std::string input(largeSize, 'A');
        std::string output;

        std::thread thread{[&] {
            while (process.is_running())
            {
                output += process.read(DEFAULT_WAIT_TIMEOUT);
            }

            output += process.read();
        }};

        process.write(input);
        process.write("\n");
        process.write("exit\n");
        thread.join();

        expect(output).to_be(input);
    });

    test("write to stopped process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .wait(DEFAULT_WAIT_TIMEOUT);

        expect([&] {
            process.write("message\n");
        })
            .to_throw<std::runtime_error>();
    });

    test("write to process that do not read", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--wait=100");

        constexpr std::size_t largeSize = 1'000'000;
        std::string input(largeSize, 'A');

        expect([&] {
            process.write(input);
        })
            .to_throw<std::runtime_error>();
    });
});
