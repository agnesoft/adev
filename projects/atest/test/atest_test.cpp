import atest;

auto main(int argc, char *argv[]) -> int
{
    atest::suite("My Suite", [] {
        atest::test("Register a test", [] {
            throw std::runtime_error{"Some error."};
        });

        atest::test("Register another test", [] {
            atest::expect(1);
        });
    });

    atest::test("Test in global suite", [] {
        atest::expect([] {
            throw std::logic_error{"Callable error"};
        });
    });

    atest::suite("Another Suite", [] {
        atest::test("Yet another test", [] {

        });
    });

    return 0;
}
