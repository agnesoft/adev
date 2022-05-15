import awinapi;
import acommandline;

//! \private
struct Inputs
{
    std::int64_t exitCode = 0;
    std::vector<std::string> echo;
    std::vector<std::string> largeEcho;
    std::vector<std::string> env;
    std::int64_t echoDelay = 0;
    std::int64_t echoInputTimeout = 0;
    std::int64_t largeEchoSize = 0;
    std::int64_t wait = 0;
    bool echoInput = false;
    bool echoWorkingDirectory = false;
};

#ifdef _MSC_VER
[[nodiscard]] auto get_env(const char *key) -> std::string
{
    char *buffer = nullptr;
    std::size_t size = 0;
    std::string value;

    if (_dupenv_s(&buffer, &size, key) == 0 && buffer != nullptr)
    {
        value = buffer;
        free(buffer); // NOLINT(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
    }

    return value;
}
#else
[[nodiscard]] auto get_env(const char *key) -> std::string
{
    return std::getenv(key);
}
#endif

auto echo(const Inputs &inputs) -> void
{
    for (const std::string &value : inputs.echo)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{inputs.echoDelay});
        std::cout << value << std::flush;
    }
}

auto echo_env(const Inputs &inputs) -> void
{
    for (const std::string &value : inputs.env)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{inputs.echoDelay});
        std::cout << ::get_env(value.c_str()) << std::flush;
    }
}

auto echo_working_directory(const Inputs &inputs) -> void
{
    if (inputs.echoWorkingDirectory)
    {
        std::cout << std::filesystem::current_path().string();
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
    if (inputs.echoInput)
    {
        const auto end = std::chrono::system_clock::now() + std::chrono::milliseconds{inputs.echoInputTimeout};

        std::atomic_bool stop{false};
        std::thread([&] {
            std::string value;

            while (std::chrono::system_clock::now() < end
                   && value != "exit")
            {
                std::cout << value << std::flush;
                std::cin >> value;
            }

            stop.store(true);
        }).detach();

        while (!stop.load() && std::chrono::system_clock::now() < end)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{0});
        }
    }
}

auto wait(const Inputs &inputs) -> void
{
    std::this_thread::sleep_for(std::chrono::milliseconds{inputs.wait});
}

auto main(int argc, char *argv[]) -> int
{
    Inputs inputs;

    try
    {
        static constexpr std::int64_t defaultEchoTimeout{500};
        ::acommandline::CommandLine parser;
        parser.option().long_name("exit-code").description("Exits with the given <value>").bind_to(&inputs.exitCode);
        parser.option().long_name("echo").description("Print the given <value>").bind_to(&inputs.echo);
        parser.option().long_name("echo-delay").description("Delay each echo by <value> milliseconds").bind_to(&inputs.echoDelay);
        parser.option().long_name("echo-env").description("Print the value of the environment variable <value>").bind_to(&inputs.env);
        parser.option().long_name("echo-input").description("Print the values received on stanard input").bind_to(&inputs.echoInput);
        parser.option().long_name("echo-input-timeout").default_value(defaultEchoTimeout).description("Print the values received on stanard input for <value> milliseconds").bind_to(&inputs.echoInputTimeout);
        parser.option().long_name("echo-large").description("A character to repeat in a large echo").bind_to(&inputs.largeEcho);
        parser.option().long_name("echo-large-size").description("Size of the large echo").bind_to(&inputs.largeEchoSize);
        parser.option().long_name("echo-working-directory").description("Prints working directory").bind_to(&inputs.echoWorkingDirectory);
        parser.option().long_name("wait").default_value(std::int64_t{0}).description("Waits for <value> milliseconds").bind_to(&inputs.wait);

        if (parser.parse(argc, argv))
        {
            echo(inputs);
            echo_env(inputs);
            echo_working_directory(inputs);
            large_echo(inputs);
            input(inputs);
            wait(inputs);
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
