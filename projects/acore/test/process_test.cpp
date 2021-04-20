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

#ifdef _MSC_VER
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

    test("output over buffer size", [] {
        constexpr int SIZE = 10000;
        std::string output(SIZE + 2, '0');
        output[SIZE] = '\r';
        output[SIZE + 1] = '\n';

        const acore::Process process{"powershell", {"-c", "\"$arr = New-Object int[] 10000; -join $arr\""}};

        expect(process.output()).toBe(output);
    });
#else
        test("command", [] {
            const acore::Process process{"/bin/bash", {"-c", "exit 0"}};

            expect(process.command()).toBe("/bin/bash");
        });

        test("arguments", [] {
            std::vector<std::string> args{"-c", "exit 0"};
            const acore::Process process{"/bin/bash", args};

            expect(process.arguments()).toBe(args);
        });

        test("exit code 0", [] {
            const acore::Process process{"/bin/bash", {"-c", "exit 0"}};

            expect(process.exitCode()).toBe(0);
        });

        test("exit code 1", [] {
            const acore::Process process{"/bin/bash", {"-c", "exit 1"}};

            expect(process.exitCode()).toBe(1);
        });

        test("output", [] {
            const acore::Process process{"/bin/bash", {"-c", "echo Hello, World!"}};

            expect(process.output()).toBe("Hello, World!\n");
        });

        test("error", [] {
            const acore::Process process{"/bin/bash", {"-c", "echo error 1>&2"}};

            expect(process.output()).toBe("error\n");
        });

        test("output over buffer size", [] {
            constexpr int SIZE = 10000;
            std::string output{"1"};

            for (int i = 2; i < SIZE; ++i)
            {
                output += ' ';
                output += std::to_string(i);
            }

            const acore::Process process{"/bin/bash", {"-c", "myarr=$(seq 10000) && echo $myarr"}};

            expect(process.output()).toBe(output);
        });
#endif
});
