#include "FileCache.hpp"

namespace FuncLib::Store
{
    FileCache::FileCache(unsigned int fileId) : _fileId(fileId) { }

    FileCache::~FileCache()
    {
        _unloader();
    }
}