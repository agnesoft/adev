import abuild.cpptokenizer;
import atest;

auto main(int argc, char *argv[]) -> int
{
    static_cast<void>(::abuild::tokenize({}));
    return ::atest::TestRunner{}.run(argc, argv);
}
