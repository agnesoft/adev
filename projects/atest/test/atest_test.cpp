import atest;

auto main(int argc, char *argv[]) -> int
{
    atest::suite("My Suite", [] {
        atest::test("Register a test", [] {
            throw std::runtime_error{"Some error."};
        });

        atest::test("Register another test", [] {
            atest::expect([] {
                return 1 + 1;
            }).toBe(2);
        });
    });

    atest::test("Test in global suite", [] {
        atest::expect([]() -> int {
            throw std::logic_error{"Callable error"};
        }).toBe(0);
    });

    atest::suite("Another Suite", [] {
        atest::test("Yet another test", [] {
            atest::expect(1 + 1).toBe(2);
        });
    });

    return 0;
}
