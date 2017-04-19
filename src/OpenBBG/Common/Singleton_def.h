#ifndef _OPENBBG__COMMON__SINGLETON_DEF_H_
#define _OPENBBG__COMMON__SINGLETON_DEF_H_

namespace openbbg {
	
template <class T>
inline
T *
Singleton<T>::Get()
{
	if (s_instance == nullptr)
		s_instance = new T();
	return s_instance;
}



}
#endif
