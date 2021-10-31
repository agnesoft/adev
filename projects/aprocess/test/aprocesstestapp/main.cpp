import acommandline;

struct Inputs
{
    std::int64_t exitCode = 0;
};

auto main(int argc, char *argv[]) -> int
{
    try
    {
        Inputs inputs;

        ::acommandline::CommandLine parser;
        parser.option().long_name("exit-code").description("Exits with the given value").bind_to(&inputs.exitCode);

        if (parser.parse(argc, argv))
        {
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
