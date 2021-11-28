import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{2000};

static const auto S = suite("read", [] { // NOLINT(cert-err58-cpp)
    test("no output", [] {
        const std::string output =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .wait(DEFAULT_WAIT_TIMEOUT)
                .read();

        expect(output).to_be(std::string{});
    });

    test("Hello, World!", [] {
        const std::string output =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arguments({"--echo=Hello", "--echo=,", "--echo=World", "--echo=!"})
                .wait(DEFAULT_WAIT_TIMEOUT)
                .read();

        expect(output).to_be(std::string{"Hello,World!"});
    });

    test("large output", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arguments({"--echo-large=A", "--echo-large-size=1000000"});

        std::string output;

        while (process.is_running())
        {
            output += process.read();
        }

        output += process.read();

        constexpr std::size_t largeSize = 1'000'000;
        assert_(output.size()).to_be(largeSize);
        expect(output == std::string(largeSize, 'A')).to_be(true);
    });

    test("partial output", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo=Hi")
                .arg("--echo=there")
                .arg("--echo-delay=10");

        expect(process.read(DEFAULT_WAIT_TIMEOUT)).to_be("Hi");
        process.wait(DEFAULT_WAIT_TIMEOUT);
    });
});
