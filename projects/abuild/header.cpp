#ifdef _MSC_VER
export module abuild : header;
export import : file;
#endif

namespace abuild
{
export class Header : public File
{
public:
    using File::File;
};
}
