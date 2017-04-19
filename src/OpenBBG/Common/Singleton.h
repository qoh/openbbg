#ifndef _OPENBBG__COMMON__SINGLETON_H_
#define _OPENBBG__COMMON__SINGLETON_H_

#define IMPLEMENT_SINGLETON(Class) \
	Class *Class::s_instance = nullptr;

namespace openbbg {

template <class T>
struct Singleton
{
	static T *s_instance;

	static T *Get();
};

}
#endif

// Definitions
#include <OpenBBG/Common/Singleton_def.h>
