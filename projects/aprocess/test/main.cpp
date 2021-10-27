import atest;
import aprocess;

using ::atest::test;

auto main(int argc, char *argv[]) -> int
{
    test("coverage", [] {
        aprocess::Process process{{.command = "echo Hello World"}};
        process.detach();
        static_cast<void>(process.exit_code());
        static_cast<void>(process.is_running());
        process.kill();
        static_cast<void>(process.read());
        static_cast<void>(process.process_setup());
        process.terminate();
        process.wait();
        static_cast<void>(process.wait(std::chrono::seconds{1}));
        process.write("");
    });

    return atest::TestRunner{}.run(argc, argv);
}
