#include <type_traits>
#include "pString.hpp"

namespace Utility
{
    template<typename TValue,typename StrType = AString>
    class JObject{
        StrType key;
        TValue  value;
    };    

    template<typename StrType = AString>
    extern AString SerialJson();
} 
