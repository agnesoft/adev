import abuild;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    abuild::BuildCache cache;
    abuild::ProjectScanner{cache, std::filesystem::current_path()};
    abuild::CodeScanner{cache};
    abuild::DependencyScanner{cache};

    for (const abuild::Warning &warning : cache.warnings())
    {
        std::cout << "[WARNING][" << warning.component << "] " << warning.what << '\n';
    }
}
