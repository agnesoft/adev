# aprocess

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)

# Overview

Cross platform process management that uses builder pattern to define the process and behaves similar to `std::thread`.

# Prerequisites

-   C++ compiler
-   C++20
-   Exceptions
-   [astl](/projects/astl/readme.md)
-   [awinapi](/projects/awinapi/readme.md) (Windows)

# Usage

At a glance:

```
std::string output;
::aprocess::create_process()
    .command("aprocesstestapp")
    .arg("--echo")
    .arg("\"Hello, World!\"")
    .read([&](std::string_view message) { output += message; })
    .wait(std::chrono::seconds{1});
std::cout << output; //Hello, World!
```

The process is created using the builder pattern starting with `create_process()` function. Upon its completing the process will be started asynchronously. You can optionally `wait()` for it right away as is the case in the example. It behaves in a similar way to `std::thread` - the process starts at construction, it can be waited and is forcefully destroyed on destruction if still running (unless it was detached).

The `::aprocess::Process` is designed to completely abstract the underlying operating system behaving in the same way on all supported platforms. However that does not apply to the run programs and their platform specifics.

## command()

The command is the executable to be run that can be relative or an absolute path or a program findable in `PATH` environment variable.

## arg(), arguments()

To specify the program arguments you can add single argument to the list with `arg()` or multiple wih `arguments()`.

**NOTE:** Arguments are NOT treated in any way. Any quotation or escaping expected by the program must be done by the user.

## env(), environment()

By default the program always inherit the environment of its calling process. With `.env()` you can prepend a new variable to the environment. The `.environment()` call will prepend the list of variables.

## detached()

By default the running process is managed by the `::aprocess::Process` object that is the result of the builder. That means that if the object goes out of scope its corresponding process will be killed on its destruction. You can specify with `.detached()` that the process should be allowed to outlive the `Process` object. All facilities of the process are still available regardless of its detachment - it can still be killed manually, written into, checked whether it is running etc. You can also `detach()` from the process when it is running.

## read()

The output of the process can be captured and used by specifying a read callback that takes `std::string_vew` into the output. The output of `STDOUT` and `STDERR` is combined.

**NOTE:** The callback is invoked in a different - reading - thread.

## write()

During process buildup you can specify that the process should be writable. After its creation it will be possible to `write()` to the process' `STDIN`.

**NOTE:** Calling `write()` does not automatically flush the underlying OS pipe nor does it cause the program to receive the message. Depending on the program implementation you may need to add a delimiter character to the message. For example a common delimiter character is `\n` (end of line) when the program reads from `STDIN` using `std::cin`.

## terminate(), kill()

If you wish to stop the process you can use `terminate()` that will send appropriate signals to the process:

-   `SIGINT` and `SIGTERM` on Unix
-   `CTRL_C_EVENT` and `WM_CLOSE` on Windows

These signals allow the process to exit gracefully if the process handle them. To forcefully stop the process you can use `kill()` which cannot be intercepted or blocked. It will:

-   `SIGKILL` on Unix
-   `TerminateProcess` on Windows

Such termination of the process might cause data loss but is the only sure way to ensure the process is stopped.

**NOTE:** The `kill()` is called in destructor of `Process` unless it was detached.

## exit_code()

You can obtain the exit code of the process with `exit_code()` which is specific to the program and to the OS. A running program will have exit code `0`. The `0` is also a customary exit code signifying successful execution of the program that has stopped. Some programs however may use the exit code for different purposes, e.g. `curl`.

**NOTE:** On Unix the exit code is limited to 0-255 value extracted from the composite status of the program with `WEXITSTATUS`. Such value does not support negative exit codes as -1 exit code of the program becomes 255.

## is_running(), wait()

You can manually check if the program is running with `is_running()`. Typically the process should be awaited with `wait()` either right after construction or after. The `wait()` requires a timeout parameter to avoid infinite deadlocks. It throws an exception if the program has not exited before timeout expired. The `wait()` will return immediately if the program is stopped or as soon as it stops so it is fine to set longer timeout periods, e.g. set 1 second if the program is expected to run in dozens of milliseconds.
