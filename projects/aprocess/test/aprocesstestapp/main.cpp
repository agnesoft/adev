import acommandline;
import awinapi;

struct Inputs
{
    std::int64_t exitCode = 0;
    std::vector<std::string> echo;
    std::vector<std::string> largeEcho;
    std::int64_t echoDelay = 0;
    std::int64_t largeEchoSize = 0;
    std::int64_t echoInputTimeout = 0;
    bool echoInput = false;
};

auto echo(const Inputs &inputs) -> void
{
    for (const std::string &value : inputs.echo)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{inputs.echoDelay});
        std::cout << value << std::flush;
    }
}

auto large_echo(const Inputs &inputs) -> void
{
    for (const std::string &character : inputs.largeEcho)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{inputs.echoDelay});
        std::cout << std::string(inputs.largeEchoSize, character.front()) << std::flush;
    }
}

auto input(const Inputs &inputs) -> void
{
    if (inputs.echoInput != 0)
    {
        const auto end = std::chrono::system_clock::now() + std::chrono::milliseconds{inputs.echoInputTimeout};

        std::atomic_bool stop{false};
        std::thread thread = std::thread([&] {
            std::string value;

            while (std::chrono::system_clock::now() < end
                   && value != "exit")
            {
                std::cout << value << std::flush;
                std::cin >> value;
            }

            stop.store(true);
        });

        while (!stop.load() && std::chrono::system_clock::now() < end)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{0});
        }
    }
}

auto main(int argc, char *argv[]) -> int
{
    Inputs inputs;

    try
    {
        ::acommandline::CommandLine parser;
        parser.option().long_name("exit-code").description("Exits with the given <value>").bind_to(&inputs.exitCode);
        parser.option().long_name("echo").description("Print the given <value>").bind_to(&inputs.echo);
        parser.option().long_name("echo-delay").description("Delay each echo by <value> milliseconds").bind_to(&inputs.echoDelay);
        parser.option().long_name("echo-input").description("Print the values received on stanard input").bind_to(&inputs.echoInput);
        parser.option().long_name("echo-input-timeout").default_value(std::int64_t{100}).description("Print the values received on stanard input for <value> milliseconds").bind_to(&inputs.echoInputTimeout);
        parser.option().long_name("echo-large").description("A character to repeat in a large echo").bind_to(&inputs.largeEcho);
        parser.option().long_name("echo-large-size").description("Size of the large echo").bind_to(&inputs.largeEchoSize);

        if (parser.parse(argc, argv))
        {
            echo(inputs);
            large_echo(inputs);
            input(inputs);
            return static_cast<int>(inputs.exitCode);
        }

        return 0;
    }
    catch (const std::exception &exception)
    {
        std::cout << exception.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Unknown exception\n";
    }

    return -1;
}
