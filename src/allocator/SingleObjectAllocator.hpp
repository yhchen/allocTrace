#pragma once

/************************************************************************/
/*                          对象内存申请器（单个对象申请）
/*
/*                   ★采用对象池的技术队大量的小内存对象的申请优化★
/*                       ★！此内存管理器不是多线程安全的！★
/*
/*   用于对连续的相同对象的小块内存申请进行优化。申请器只提供申请对象的功能并且能够将内存
/* 申请的效率进行最大化提升，提供释放对象所占用内存的功能。所有对象所占用的内存将在申请
/* 器销毁时才被释放。
/*
/************************************************************************/

#include "container/BaseList.h"

namespace alloctrace
{
	namespace allocator
	{
		using namespace container;

		template <class T>
		class CSingleObjectAllocator :
			protected CBaseList <T*>
		{
		public:
			typedef CBaseList<T*> Inherited;
		private:
			struct DataBlock
			{
				DataBlock*	pPrev;
			}		*m_pDataBlock;
		public:
			enum
			{
				AllocObjectCountOneTime = 256,
			};
			//申请属性指针
			T* allocObject()
			{
				T* result;
				INT_PTR nCount = Inherited::count();

				if (nCount <= 0)
				{
					//static const INT_PTR nAllocCount = 4096;
					DataBlock *pBlock = (DataBlock*)malloc(sizeof(DataBlock) + sizeof(T) * AllocObjectCountOneTime);
					memset(pBlock, 0, sizeof(DataBlock) + sizeof(T) * AllocObjectCountOneTime);
					//将对象指针全部添加到空闲列表中
					result = (T*)(pBlock + 1);
					if (Inherited::maxCount() < AllocObjectCountOneTime)
						reserve(AllocObjectCountOneTime);
					for (INT_PTR i = 0; i < AllocObjectCountOneTime; ++i)
					{
						(*this)[i] = result;
						result++;
					}
					nCount = AllocObjectCountOneTime;
					//标记上内存块的上一个内存块
					pBlock->pPrev = m_pDataBlock;
					m_pDataBlock = pBlock;
				}
				//返回列表中的最后一个对象
				nCount--;
				result = (*this)[nCount];
				Inherited::trunc(nCount);
				return result;
			}
			//释放一个对象
			inline void freeObject(T* pObject)
			{
				add(pObject);
			}
			//释放并清除所有对象的内存，将使得所有已申请的对象无效！
			inline void freeAll()
			{
				Inherited::empty();
				DataBlock *pBlock, *pPrev;
				pBlock = m_pDataBlock;
				while (pBlock)
				{
					pPrev = pBlock->pPrev;
					free(pBlock);
					pBlock = pPrev;
				}
				m_pDataBlock = NULL;
			}
			CSingleObjectAllocator()
			{
				m_pDataBlock = NULL;
			}
			~CSingleObjectAllocator()
			{
				freeAll();
			}
		};

	}
}

