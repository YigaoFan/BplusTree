#include "FileCache.hpp"

namespace FuncLib::Store
{
    FileCache::FileCache(id_int fileId) : _fileId(fileId) { }

    FileCache::~FileCache()
    {
        _unloader();
    }
}