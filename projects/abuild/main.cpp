import abuild;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    const std::filesystem::path cacheFile = std::filesystem::current_path() / "build" / ".abuild";
    std::filesystem::remove(cacheFile);
    abuild::BuildCache cache{cacheFile};
    abuild::Settings settings{cache};
    abuild::ToolchainScanner{cache};
    abuild::ProjectScanner{std::filesystem::current_path(), cache, settings};
}
