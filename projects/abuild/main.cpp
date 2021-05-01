import abuild;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    std::filesystem::remove(std::filesystem::current_path() / "build" / ".abuild");
    abuild::BuildCache cache;
    abuild::Settings settings{cache};
    abuild::Toolchains toolchains{cache};
    abuild::Projects projects{cache, settings};
    abuild::Dependencies dependencies{cache};
}
