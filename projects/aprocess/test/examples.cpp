// clang-format off
//! [[synchronous process]]
aprocess::Process process{{.command = "echo Hello World"}};
process.wait();
std::cout << process.read();
//! [[synchronous process]]
// clang-format on

// clang-format off
//! [[real time output]]
aprocess::Process process{{.command = "echo Hello World"}};
while (process.is_running()) //beware of possible deadlock, consider offloading this to a new thread
{
    std::cout << process.read();
}
std::cout << process.read(); //make sure you get the rest of the output after the process stopped
//! [[real time output]]
// clang-format on

// clang-format off
//! [[input]]
aprocess::Process process{{.command = "echo Hello && read name && echo $name"}};
process.input("AProcess");
process.wait();
//! [[input]]
// clang-format on
