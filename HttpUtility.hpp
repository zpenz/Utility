#pragma once

#include "pString.hpp"
#include "Contain.h"

namespace Utility
{
    using namespace Contain;
    extern AString::KeyValuePair<AString> CutUrl(const AString& url);
    extern AString FormPost(const AString& url,Linker<AString> list,long timeout = 99999);     
} // namespace Utility
