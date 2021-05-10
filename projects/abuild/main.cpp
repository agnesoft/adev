import abuild;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    const std::filesystem::path cacheFile = std::filesystem::current_path() / "build" / ".abuild";

    std::filesystem::remove(cacheFile);

    abuild::BuildCache cache{cacheFile};
    abuild::ToolchainScanner{cache};
    abuild::ProjectScanner{cache, std::filesystem::current_path()};
    abuild::CodeScanner{cache};
    abuild::DependencyScanner{cache};
}
