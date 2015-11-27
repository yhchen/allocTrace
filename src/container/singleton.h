#include <assert.h>
#ifndef __ALLOC_TRACE_SINGLETON_H__
#define __ALLOC_TRACE_SINGLETON_H__

namespace alloctrace
{

	template<typename _Type>
	class singleton
	{
	public:
		// 初始化单例
		static void initialize()
		{
			assert(s_Type == NULL);
			s_Type = new _Type;
		}

		// 释放单例
		static void uninitialize()
		{
			if (s_Type)
			{
				delete s_Type;
				s_Type = NULL;
			}
		}

		static _Type* instance()
		{
			return s_Type;
		}

	protected:
		singleton() {}
		~singleton() {}
		static _Type* s_Type;
	};

	template<typename _Type>
	_Type* singleton<_Type>::s_Type = NULL;

}

#endif // __ALLOC_TRACE_SINGLETON_H__
