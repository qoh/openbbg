#pragma once

#include <OpenBBG/InternalConfig.h>

#define USE_EASTL 0

// Assert on all build configs
#ifdef NDEBUG
#undef NDEBUG
#endif

#if USE_EASTL
using namespace eastl;
#else
using namespace std;
#endif