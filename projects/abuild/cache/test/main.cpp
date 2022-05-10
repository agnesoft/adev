import atest;
import abuild.cache;

namespace abuild
{
auto operator<<(std::ostream &stream, const File &file) -> std::ostream &
{
    return stream << file.path << " (" << file.timestamp << ", " << file.outdated << ')';
}

[[nodiscard]] auto operator==(const File &left, const File &right) -> bool
{
    return left.path == right.path && left.timestamp == right.timestamp && left.outdated == right.outdated;
}
}

auto main(int argc, char *argv[]) -> int
{
    return ::atest::TestRunner{}.run(argc, argv);
}
