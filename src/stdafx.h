// OpenBBG
#include <OpenBBG/Config.h>

#ifdef _WIN32
#include <Windows.h>
#endif


// Standard Library
#include <cassert>
#include <inttypes.h>

#include <limits>
#include <thread>
//#include <fstream>
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

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// OpenBBG
#include <OpenBBG/Game.h>
#include <OpenBBG/Log.h>