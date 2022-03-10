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

        pool.wait(std::chrono::milliseconds{100});

        expect(result).to_be(1);
    });

    test("multiple jobs", [] {
        std::atomic_int result = 0;

        ::athreadpool::ThreadPool pool{2};

        for (std::size_t i = 0; i < 100; ++i)
        {
            pool.run([&] {
                ++result;
            });
        }

        pool.wait(std::chrono::milliseconds{100});

        expect(result.load()).to_be(100);
    });

    test("wait timeout on queue", [] {
        std::atomic_int result = 0;

        ::athreadpool::ThreadPool pool{2};

        for (std::size_t i = 0; i < 10; ++i)
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
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        });

        expect([&] { pool.wait(std::chrono::milliseconds{1}); }).to_throw<std::runtime_error>("ThreadPool::wait() timed out");
    });

    test("destroyed with pending jobs", [] {
        std::atomic_int result = 0;

        {
            ::athreadpool::ThreadPool pool{2};

            for (std::size_t i = 0; i < 10; ++i)
            {
                pool.run([&] {
                    ++result;
                    std::this_thread::sleep_for(std::chrono::milliseconds{10});
                });
            }
        }

        expect(result.load()).to_be(2);
    });
});
