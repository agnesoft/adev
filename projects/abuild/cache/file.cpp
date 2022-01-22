#ifndef __clang__
export module abuild.cache : file;
export import astl;
#endif

namespace abuild
{
//! The `File` is a base class for all files that
//! are being tracked by the `Cache`.
export class File
{
public:
    //! Constructs `File` with `path` and sets the
    //! outdated flag to `true`.
    explicit File(std::filesystem::path path) noexcept :
        File{std::move(path), true}
    {
    }

    //! Constructs `File` with `path` and sets the
    //! outdated flag to `outdated`.
    File(std::filesystem::path path, bool outdated) noexcept :
        filePath{std::move(path)},
        outdated{outdated}
    {
    }

    //! Returns file extension. E.g. `.cpp`
    [[nodiscard]] auto extension() const -> std::string
    {
        return this->filePath.filename().extension().string();
    }

    //! Returns `true` if the file was constructed
    //! as outdated or `false` otherwise.
    [[nodiscard]] auto is_outdated() const -> bool
    {
        return this->outdated;
    }

    //! Returns the path of the file.
    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return this->filePath;
    }

private:
    std::filesystem::path filePath;
    bool outdated = false;
};
}
