import atest;
import athreadpool;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("ThreadPool", [] { // NOLINT(cert-err58-cpp)
    test("single job", [] {
        int result = 0;

        ::athreadpool::ThreadPool pool;
        pool.run([&] {
            result = 1;
        });

        static constexpr std::chrono::milliseconds timeout{100};
        pool.wait(timeout);

        expect(result).to_be(1);
    });

    test("multiple jobs", [] {
        static constexpr int jobs = 100;
        std::atomic_int result = 0;

        ::athreadpool::ThreadPool pool{2};

        for (int i = 0; i < jobs; ++i)
        {
            pool.run([&] {
                ++result;
            });
        }

        static constexpr std::chrono::milliseconds timeout{100};
        pool.wait(timeout);

        expect(result.load()).to_be(jobs);
    });

    test("wait timeout on queue", [] {
        static constexpr int jobs = 100;
        std::atomic_int result = 0;

        ::athreadpool::ThreadPool pool{2};

        for (int i = 0; i < jobs; ++i)
        {
            pool.run([&] {
                ++result;
            });
        }

        expect([&] { pool.wait(std::chrono::milliseconds{1}); }).to_throw<std::runtime_error>("ThreadPool::wait() timed out");
    });

    test("wait timeout on job", [] {
        std::atomic_int result = 0;

        ::athreadpool::ThreadPool pool{2};

        pool.run([&] {
            ++result;

            static constexpr std::chrono::milliseconds sleepTime{10};
            std::this_thread::sleep_for(sleepTime);
        });

        expect([&] { pool.wait(std::chrono::milliseconds{1}); }).to_throw<std::runtime_error>("ThreadPool::wait() timed out");
    });

    test("destroyed with pending jobs", [] {
        static constexpr int jobs = 10;
        std::atomic_int result = 0;

        {
            ::athreadpool::ThreadPool pool{2};

            for (int i = 0; i < jobs; ++i)
            {
                pool.run([&] {
                    ++result;

                    static constexpr std::chrono::milliseconds sleepTime{10};
                    std::this_thread::sleep_for(sleepTime);
                });
            }
        }

        expect(result.load()).to_be(2);
    });
});
