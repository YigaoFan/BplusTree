#include "../../Basic/Exception.hpp"
#include "FileResource.hpp"

namespace FuncLib::Store
{
    using ::Basic::InvalidAccessException;

    thread_local shared_ptr<File> FileResource::CurrentFile = nullptr;

    shared_ptr<File> FileResource::GetCurrentThreadFile()
    {
        if (CurrentFile == nullptr)
        {
            throw InvalidAccessException("Null current file");
        }

        return CurrentFile;
    }

    void FileResource::SetCurrentThreadFile(shared_ptr<File> file)
    {
        CurrentFile = file;
    }
}