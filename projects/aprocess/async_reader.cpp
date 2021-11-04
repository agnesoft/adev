#ifndef __clang__
module aprocess : async_reader;
import awinapi;
import : process_setup;
#endif

#ifdef _WIN32
namespace aprocess
{
class AsyncReader
{
public:
    AsyncReader(::HANDLE &readHandle, const ProcessSetup &setup, Process &process) :
        thread{[&] {
            constexpr std::size_t BUFFER_SIZE = 65536;
            std::string buffer(BUFFER_SIZE, char{});
            DWORD bytesRead = 0;

            while (::ReadFile(readHandle,
                              static_cast<LPVOID>(buffer.data()),
                              static_cast<DWORD>(BUFFER_SIZE),
                              &bytesRead,
                              nullptr)
                   != FALSE)
            {
                setup.read(std::string_view{buffer.data(), static_cast<std::size_t>(bytesRead)}, process);
            }
        }}
    {
    }

    AsyncReader(const AsyncReader &other) = delete;
    AsyncReader(AsyncReader &&other) noexcept = default;

    ~AsyncReader()
    {
        this->thread.join();
    }

    auto operator=(const AsyncReader &other) -> AsyncReader & = delete;
    auto operator=(AsyncReader &&other) noexcept -> AsyncReader & = default;

private:
    std::thread thread;
};
}
#endif
