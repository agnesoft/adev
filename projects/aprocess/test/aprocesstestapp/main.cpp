import acommandline;

struct Inputs
{
    std::int64_t exitCode = 0;
    std::vector<std::string> echo;
    std::vector<std::string> largeEcho;
    std::int64_t echoDelay = 0;
    std::int64_t largeEchoSize = 0;
    std::int64_t timeout = 500;
    bool input = false;
};

auto main(int argc, char *argv[]) -> int
{
    try
    {
        Inputs inputs;

        ::acommandline::CommandLine parser;
        parser.option().long_name("exit-code").description("Exits with the given value").bind_to(&inputs.exitCode);
        parser.option().long_name("timeout").default_value(std::int64_t{500}).description("Maximum execution time").bind_to(&inputs.timeout);
        parser.option().long_name("echo").description("Print the given value").bind_to(&inputs.echo);
        parser.option().long_name("echo-delay").description("Delay each echo by value milliseconds").bind_to(&inputs.echoDelay);
        parser.option().long_name("echo-input").description("Print the values received on stanard input").bind_to(&inputs.input);
        parser.option().long_name("echo-large").description("A character to repeat in a large echo").bind_to(&inputs.largeEcho);
        parser.option().long_name("echo-large-size").description("Size of the large echo").bind_to(&inputs.largeEchoSize);

        if (parser.parse(argc, argv))
        {
            for (const std::string &value : inputs.echo)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{inputs.echoDelay});
                std::cout << value << std::flush;
            }

            for (const std::string &character : inputs.largeEcho)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{inputs.echoDelay});
                std::cout << std::string(inputs.largeEchoSize, character.front()) << std::flush;
            }

            if (inputs.input)
            {
                const auto end = std::chrono::system_clock::now() + std::chrono::milliseconds{inputs.timeout};
                std::string value;

                while (std::chrono::system_clock::now() < end && value != "exit")
                {
                    if (std::cin.peek() != std::ifstream::traits_type::eof())
                    {
                        std::cout << value << std::flush;
                        std::cin >> value;
                    }
                }
            }

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
