import atest;
import aprocess;
import aprocesstest;

using ::atest::test;

auto main(int argc, char *argv[]) -> int
{
    ::set_path(argv[0]);
    return ::atest::TestRunner{}.run(argc, argv);
}
