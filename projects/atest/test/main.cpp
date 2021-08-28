import atest;

auto main(int argc, char *argv[]) -> int
{
    const auto sourceLocation = std::source_location::current();
    std::cout << argc << ' ' << argv[0] << " at " << sourceLocation.file_name() << ':' << sourceLocation.line() << '\n';
}
