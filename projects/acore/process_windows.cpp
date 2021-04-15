module;

#pragma warning(push)
#pragma warning(disable : 5105)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma warning(pop)

module acore : process_windows;

import : acore_common;

namespace acore
{
class WindowsProcess
{
public:
    WindowsProcess() :
        mSecurityAttributes{createSecurityAttributes()},
        mOutputHandles{createOutputHandles()},
        mStartupInfo{createStartupInfo()},
        mProcessInfo{createProcessInfo()}
    {
    }

    WindowsProcess(const WindowsProcess &other) = delete;
    WindowsProcess(WindowsProcess &&other) noexcept = default;

    auto start(std::string *command, const std::string &workingDirectory) -> void
    {
        if (::CreateProcess(nullptr,
                            &command->front(),
                            nullptr,
                            nullptr,
                            TRUE,
                            0,
                            nullptr,
                            workingDirectory.c_str(),
                            &mStartupInfo,
                            &mProcessInfo)
            == FALSE)
        {
            throw std::runtime_error{"Failed to start process '" + *command + "': " + lastError()};
        }
    }

    auto wait() -> int
    {
        waitForProcess();
        closeHandle(&mOutputHandles.stdoutWrite);
        closeHandle(&mOutputHandles.stderrWrite);
        closeHandle(&mProcessInfo.hProcess);
        closeHandle(&mProcessInfo.hThread);
        return getExitCode();
    }

    [[nodiscard]] auto stdout() -> std::string
    {
        return readPipe(mOutputHandles.stdoutRead);
    }

    [[nodiscard]] auto stderr() -> std::string
    {
        return readPipe(mOutputHandles.stderrRead);
    }

    ~WindowsProcess()
    {
        closeHandleNothrow(mOutputHandles.stdoutRead);
        closeHandleNothrow(mOutputHandles.stderrRead);
        closeHandleNothrow(mOutputHandles.stdoutWrite);
        closeHandleNothrow(mOutputHandles.stderrWrite);
        closeHandleNothrow(mProcessInfo.hProcess);
        closeHandleNothrow(mProcessInfo.hThread);
    }

    auto operator=(const WindowsProcess &other) -> WindowsProcess & = delete;
    auto operator=(WindowsProcess &&other) noexcept -> WindowsProcess & = default;

private:
    struct OutputHandles
    {
        HANDLE stdoutRead = nullptr;
        HANDLE stdoutWrite = nullptr;
        HANDLE stderrRead = nullptr;
        HANDLE stderrWrite nullptr;
    };

    [[nodiscard]] static auto readPipe(HANDLE handle) -> std::string
    {
        std::string content;
        const DWORD BUFFER_SIZE = 65536;
        char buffer[BUFFER_SIZE] = {};
        DWORD bytesRead = 0;

        while (::ReadFile(handle,
                          static_cast<LPVOID>(buffer),
                          BUFFER_SIZE,
                          &bytesRead,
                          nullptr)
                   == TRUE
               && bytesRead != 0)
        {
            content.append(buffer, bytesRead);
        }

        return content;
    }

    [[nodiscard]] static auto lastError() -> std::string
    {
        return std::to_string(::GetLastError());
    }

    auto closeHandle(HANDLE *handle) -> void
    {
        if (::CloseHandle(*handle) == FALSE)
        {
            throw std::runtime_error{"Failed to close handle: " + lastError()};
        }

        *handle = nullptr;
    }

    auto closeHandleNothrow(HANDLE handle) -> void
    {
        if (handle)
        {
            CloseHandle(handle);
        }
    }

    auto waitForProcess() -> void
    {
        switch (WaitForSingleObject(mProcessInfo.hProcess, INFINITE))
        {
        case WAIT_OBJECT_0:
            return;
        case WAIT_TIMEOUT:
            throw std::runtime_error{"Timed out waiting for process"};
        case WAIT_ABANDONED:
            [[fallthrough]];
        case WAIT_FAILED:
            [[fallthrough]];
        default:
            throw std::runtime_error{"Failed to wait for process: " + lastError()};
        }
    }

    [[nodiscard]] auto getExitCode() -> int
    {
        DWORD exitCode = {};

        if (::GetExitCodeProcess(mProcessInfo.hProcess, &exitCode) == FALSE)
        {
            throw std::runtime_error{"Failed to get exit code from process: " + lastError()};
        }

        return static_cast<int>(exitCode);
    }

    static auto createPipe(HANDLE *read, HANDLE *write) -> void
    {
        if (::CreatePipe(read, write, &mSecurityAttributes, 0) == FALSE)
        {
            throw std::runtime_error{std::string{"Failed to create pipe: " + lastError()}};
        }
    }

    static auto createSecurityAttributes() -> SECURITY_ATTRIBUTES
    {
        SECURITY_ATTRIBUTES attributes;
        ::ZeroMemory(&attributes, sizeof(attributes));
        attributes.nLength = sizeof(attributes);
        attributes.bInheritHandle = TRUE;
        return attributes;
    }

    auto createOutputHandles() -> Handles
    {
        OutputHandles handles;
        createPipe(&handles.stdoutRead, &handles.stdoutWrite);
        createPipe(&handles.stderrRead, &handles.stderrWrite);
        return handles;
    }

    auto creteStartupInfo() -> STARTUPINFO
    {
        STARTUPINFO info;
        ::ZeroMemory(&info, sizeof(info));
        info.cb = sizeof(info);
        info.hStdOutput = mStdoutWrite;
        info.hStdError = mStderrWrite;
        info.dwFlags |= STARTF_USESTDHANDLES;
        return info;
    }

    static auto createProcessInfo() -> PROCESS_INFORMATION
    {
        PROCESS_INFORMATION info;
        ::ZeroMemory(&info, sizeof(info));
        return info;
    }

    SECURITY_ATTRIBUTES mSecurityAttributes;
    OutputHandles mOutputHandles;
    STARTUPINFO mStartupInfo;
    PROCESS_INFORMATION mProcessInfo;
};
}
