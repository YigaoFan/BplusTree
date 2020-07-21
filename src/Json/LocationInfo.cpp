#include "LocationInfo.hpp"

namespace Json
{
    LocationInfo::LocationInfo(string_view str, size_t location) : AllString(str), Location(location)
    { }

    char LocationInfo::CharAtLocation() const
    {
        return AllString[Location];
    }

    string LocationInfo::StringAround() const
    {
        return string(Location >= 2 ? AllString.substr(Location - 2, 2) : AllString.substr(Location, 2));
    }
}
