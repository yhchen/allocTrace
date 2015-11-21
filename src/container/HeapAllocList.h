#ifndef __ALLOC_TRACE_HEAPALLOCLIST_H__
#define __ALLOC_TRACE_HEAPALLOCLIST_H__

#include "BaseList.h"

namespace alloctrace
{
	namespace container
	{

		template<typename _Type>
		class HeapAllocList : public CBaseList<_Type>
		{
		public:
			typedef CBaseList<_Type> super;
		public:
			virtual ~HeapAllocList()
			{
				empty();
			}
			virtual void empty()
			{
				clear();
				m_tMaxCount = 0;
				if (m_pData)
				{
					HeapFree(GetProcessHeap(), 0, m_pData);
					m_pData = 0;
				}
			}
			virtual void reserve(INT_PTR count)
			{
				if (count > m_tCount && count != m_tMaxCount)
				{
					m_tMaxCount = count;
					if (m_pData)
						m_pData = (AllocRecordEx*)HeapReAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, m_pData, sizeof(AllocRecordEx) * count);
					else m_pData = (AllocRecordEx*)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sizeof(AllocRecordEx) * count);
				}
			}
		};

	}
}

#endif // __ALLOC_TRACE_HEAPALLOCLIST_H__
