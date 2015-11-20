#ifndef __SG2D_SMART_OBJECT_H__
#define __SG2D_SMART_OBJECT_H__

namespace SG2D
{
	/**
	 * SG2D SmartObject 智能对象类，自动调用Object对象retain和release管理生命期
	 *
	 */
	template <class T = Object>
	class SmartObject
	{
	public:
		T* m_ptr;

	public:
		SmartObject()
		{
			m_ptr = NULL;
		}
		SmartObject(T* ptr)
		{
			m_ptr = NULL;
			operator = (ptr);
		}
		SmartObject(SmartObject<T>& b)
		{
			m_ptr = NULL;
			operator = (b.m_ptr);
		}
		~SmartObject()
		{
			if (m_ptr)
			{
				m_ptr->release();
				m_ptr = NULL;
			}
		}
		inline operator T* ()
		{
			return m_ptr;
		}
		inline operator const T* () const
		{
			return m_ptr;
		}
		inline T* operator -> ()
		{
			return m_ptr;
		}
		inline const T* operator -> () const
		{
			return m_ptr;
		}
		inline SmartObject<T>& operator = (SmartObject<T> &b)
		{
			if (m_ptr != b.m_ptr)
			{
				if (m_ptr) m_ptr->release();
				m_ptr = b.m_ptr;
				if (m_ptr) m_ptr->retain();
			}
			return *this;
		}
		inline SmartObject<T>& operator = (T* ptr)
		{
			if (m_ptr != ptr)
			{
				if (m_ptr) m_ptr->release();
				m_ptr = ptr;
				if (m_ptr) m_ptr->retain();
			}
			return *this;
		}
		inline SmartObject<T>& create()
		{
			T* object = new T();
			if (m_ptr)
				m_ptr->release();
			m_ptr = object;
			return *this;
		}
	};

	template <class T = Object, int N = 1>
	class SmartObjectArray
	{
	public:
		typedef T* TypePtr;
		typedef TypePtr TypeArray[N];

	public:
		SmartObject<T> m_arr[N];

	public:
		SmartObjectArray(){};
		SmartObjectArray(SmartObjectArray<T, N> &b)
		{
			operator = (b);
		}
		SmartObjectArray(T* arr)
		{
			operator = (arr);
		}
		SmartObjectArray(T** arr)
		{
			operator = (arr);
		}
		inline operator SmartObject<T>* ()
		{
			return m_arr;
		}
		inline SmartObjectArray<T, N>& operator = (SmartObjectArray<T> &b)
		{
			for (int i=0; i<N; ++i)
			{
				m_arr[i] = b.m_arr[i];
			}
			return *this;
		}
		inline SmartObjectArray<T, N>& operator = (T* ptr)
		{
			for (int i=0; i<N; ++i)
			{
				m_arr[i] = &ptr[i];
			}
			return *this;
		}
		inline SmartObjectArray<T, N>& operator = (T** ptr)
		{
			for (int i=0; i<N; ++i)
			{
				m_arr[i] = ptr[i];
			}
			return *this;
		}
	};
}

#endif