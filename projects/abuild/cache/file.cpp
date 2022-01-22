#ifndef __clang__
export module abuild.cache : file;
export import astl;
#endif

namespace abuild
{
export class File
{
public:
    explicit File(std::filesystem::path path) noexcept :
        File{std::move(path), true}
    {
    }

    File(std::filesystem::path path, bool outdated) noexcept :
        filePath{std::move(path)},
        outdated{outdated}
    {
    }

    [[nodiscard]] auto extension() const -> std::string
    {
        return this->filePath.filename().extension().string();
    }

    [[nodiscard]] auto is_outdated() const -> bool
    {
        return this->outdated;
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return this->filePath;
    }

private:
    std::filesystem::path filePath;
    bool outdated = false;
};
}
