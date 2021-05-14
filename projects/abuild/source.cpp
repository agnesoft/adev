#ifdef _MSC_VER
export module abuild : source;
export import : file;
#endif

namespace abuild
{
export class Source : public File
{
public:
    using File::File;
};
}
