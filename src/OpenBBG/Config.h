#pragma once

#include <OpenBBG/InternalConfig.h>

#define USE_EASTL 0

// Assert on all build configs
#ifdef NDEBUG
#undef NDEBUG
#endif

// Standard Library
#include <cassert>
#include <inttypes.h>

#include <thread>
#include <mutex>
#include <condition_variable>

#if USE_EASTL
// EASTL
#include <EASTL/string.h>
#include <EASTL/deque.h>
#include <EASTL/map.h>
#include <EASTL/queue.h>
#include <EASTL/memory.h>
#include <EASTL/vector.h>
#include <EASTL/chrono.h>

using namespace eastl;
#else
// Standard Library
#include <algorithm>
#include <string>
#include <deque>
#include <map>
#include <queue>
#include <memory>
#include <vector>
#include <chrono>

using namespace std;
#endif


namespace openbbg {
}
