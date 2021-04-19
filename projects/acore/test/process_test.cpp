import atest;
import acore;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("acore::Process", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<acore::Process>).toBe(false);
        expect(std::is_copy_constructible_v<acore::Process>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<acore::Process>).toBe(true);
        expect(std::is_copy_assignable_v<acore::Process>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<acore::Process>).toBe(true);
        expect(std::is_nothrow_destructible_v<acore::Process>).toBe(true);
    });

    test("command", [] {
        const acore::Process process{"cmd", {"/c", "\"exit 0\""}};

        expect(process.command()).toBe("cmd");
    });

    test("arguments", [] {
        std::vector<std::string> args{"/c", "\"echo Hello, World!\""};
        const acore::Process process{"cmd", args};

        expect(process.arguments()).toBe(args);
    });

    test("exit code 0", [] {
        const acore::Process process{"cmd", {"/c", "\"exit 0\""}};

        expect(process.exitCode()).toBe(0);
    });

    test("exit code 1", [] {
        const acore::Process process{"cmd", {"/c", "\"exit 1\""}};

        expect(process.exitCode()).toBe(1);
    });

    test("output", [] {
        const acore::Process process{"cmd", {"/c", "\"echo Hello, World!\""}};

        expect(process.output()).toBe("Hello, World!\r\n");
    });

    test("error", [] {
        const acore::Process process{"cmd", {"/c", "\"echo error 1>&2\""}};

        expect(process.output()).toBe("error \r\n");
    });
});
