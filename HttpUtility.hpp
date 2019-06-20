#pragma once

#include "pString.hpp"

namespace Utility
{
    extern AString FormPost(const AString& url,const AString& data,long timeout = 99999);     
} // namespace Utility
