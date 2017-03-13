#pragma once

#define USE_EASTL 0

// Assert on all build configs
#ifdef NDEBUG
#undef NDEBUG
#endif

// Standard Library
#include <cassert>
#include <inttypes.h>

#if USE_EASTL
// EASTL
#include <EASTL/string.h>
#include <EASTL/deque.h>
#include <EASTL/map.h>
#include <EASTL/queue.h>

using namespace eastl;
#else
// Standard Library
#include <string>
#include <deque>
#include <map>
#include <queue>

using namespace std;
#endif


namespace openbbg {
}