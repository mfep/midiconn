#pragma once

#ifdef MC_BUILD_WITH_TRANSLATIONS
    #include "libintl.h"
#endif

namespace mc
{

inline const char* gettext(const char* key)
{
#ifdef MC_BUILD_WITH_TRANSLATIONS
    return ::gettext(key);
#else
    return key;
#endif
}

} // namespace mc
