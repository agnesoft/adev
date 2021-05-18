#ifdef _MSC_VER
export module abuild_test_utilities : test_project_with_content;

export import : test_project;
#endif

export class TestProjectWithContent : public TestProject
{
public:
    TestProjectWithContent(const std::filesystem::path &root, const std::vector<std::pair<std::filesystem::path, std::string>> &files)
    {
        remove_all(root);
        std::filesystem::create_directories(root);
        getRoot() = std::filesystem::canonical(root);

        for (const std::pair<std::filesystem::path, std::string> &file : files)
        {
            const std::filesystem::path filePath = projectRoot() / file.first;
            std::filesystem::create_directories(filePath.parent_path());
            std::ofstream{filePath} << file.second;
        }
    }
};