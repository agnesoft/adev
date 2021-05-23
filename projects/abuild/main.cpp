import abuild;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) -> int
{
    try
    {
        abuild::BuildCache cache;

        {
            std::cout << "ProjectScanner... ";
            auto start = std::chrono::steady_clock::now();
            abuild::ProjectScanner scanner{cache, std::filesystem::current_path()};
            auto end = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start) << '\n';
        }

        {
            std::cout << "CodeScanner... ";
            auto start = std::chrono::steady_clock::now();
            abuild::CodeScanner scanner{cache};
            auto end = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start) << '\n';
        }

        {
            std::cout << "DependencyScanner... ";
            auto start = std::chrono::steady_clock::now();
            abuild::DependencyScanner scanner{cache};
            auto end = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start) << '\n';
        }

        std::cout << "\nWarnings: " << cache.warnings().size();
        std::cout << "\nSources: " << cache.sources().size();
        std::cout << "\nHeaders: " << cache.headers().size();
        std::cout << "\nProjects: " << cache.projects().size();
        std::cout << "\nModules: " << cache.modules().size();

        for (const abuild::Warning &warning : cache.warnings())
        {
            std::cout << warning.component << ": " << warning.what << '\n';
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    catch (...)
    {
        std::cout << "UNEXPECTED FAILURE";
    }
}
