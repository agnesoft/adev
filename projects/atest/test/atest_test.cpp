import atest;

auto main(int argc, char *argv[]) -> int
{
    atest::suite("My Suite", [] {
        atest::test("Register a test", [] {

        });

        atest::test("Register another test", [] {

        });
    });

    atest::test("Test in global suite", [] {

    });

    atest::suite("Another Suite", [] {
        atest::test("Yet another test", [] {

        });
    });

    return 0;
}
