import acommandline;

struct Inputs
{
    std::int64_t exitCode = 0;
    std::vector<std::string> echo;
    std::int64_t delay = 0;
    bool echoInput = false;
};

auto main(int argc, char *argv[]) -> int
{
    try
    {
        Inputs inputs;

        ::acommandline::CommandLine parser;
        parser.option().long_name("exit-code").description("Exits with the given value").bind_to(&inputs.exitCode);
        parser.option().long_name("echo").description("Print the given value").bind_to(&inputs.echo);
        parser.option().long_name("echo-delay").description("Delay each echo by value milliseconds").bind_to(&inputs.delay);
        parser.option().long_name("echo-input").description("Print the values received on stanard input").bind_to(&inputs.echoInput);

        if (parser.parse(argc, argv))
        {
            for (const std::string &value : inputs.echo)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{inputs.delay});
                std::cout << value << std::flush;
            }

            if (inputs.echoInput)
            {
                std::string value;

                while (value != "exit")
                {
                    std::cout << value << std::flush;
                    std::cin >> value;
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
