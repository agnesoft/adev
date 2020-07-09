#include "pch.hpp"

#include "PersistentDataIndexMap.hpp"

namespace afile
{
PersistentDataIndexMap::PersistentDataIndexMap(File *file) :
    acore::DataIndexMapBase<PersistentDataIndexMapData>(file)
{
}

PersistentDataIndexMap::PersistentDataIndexMap(File *file, acore::size_type index) :
    acore::DataIndexMapBase<PersistentDataIndexMapData>(file, index)
{
}

File *PersistentDataIndexMap::file() const noexcept
{
    return storage()->file();
}

acore::size_type PersistentDataIndexMap::fileIndex() const noexcept
{
    return storage()->fileIndex();
}

acore::DataIndexMap PersistentDataIndexMap::toDataIndexMap() const
{
    //    acore::DataIndexMap map;

    //    for (auto it = cbegin(); it != cend(); ++it)
    //    {
    //        map.insert((*it).key, (*(*it).value).key, (*(*it).value).value);
    //    }

    //    return map;
    return {};
}
} // namespace afile
