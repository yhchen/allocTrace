#pragma once

/************************************************************************/
/*                                对象内存申请器
/*
/*      ★仅适用于大量的小内存对象的申请优化，且对象是长时间使用而不需释放的场合★
/*                       ★！此内存管理器不是多线程安全的！★
/*
/*   用于对连续的相同对象的小块内存申请进行优化。申请器只提供申请对象的功能并且能够将内存
/* 申请的效率进行最大化提升，不提供释放对象所占用内存的功能。所有对象所占用的内存将在申请
/* 器销毁时才被释放。
/*
/************************************************************************/

namespace alloctrace
{
	namespace allocator
	{

		template <typename T>
		class CDBGObjectAllocator
		{
		public:
			//内存块描述结构，用于查询内存块信息
			struct DataBlockDesc
			{
				const char* lpBaseAddress;//内存块指针地址
				size_t dwBlockSize;		  //内存块大小
			};

		private:
			//内存块链表节点
			struct DataBlock
			{
				T*			pFreePtr;	//指向空闲的对象
				T*			pEndPtr;	//指向对象内存块的结束
				DataBlock*	pPrev;
			}		*m_pDataBlock;

		public:
			enum {
				AllocObjectCountOneTime = 1024
			};
			//申请属性指针
			T* allocObjects(const INT_PTR nCount = 1)
			{
				T* result;
				DataBlock *pBlock = m_pDataBlock;
				//尝试从当前内存块中申请nCount的对象，如果内存块中没有足够的对象，则向之前的内存块中查找是否具有足够的空闲对象
				if (!pBlock || pBlock->pEndPtr - pBlock->pFreePtr < nCount)
				{
					//向之前的内存块中查找是否具有足够的空闲对象
					while (pBlock && (pBlock = pBlock->pPrev))
					{
						if (pBlock->pEndPtr - pBlock->pFreePtr >= nCount)
							break;
					}
					//如果所有内存块中都没有足够的对象则申请新内存块
					if (pBlock == NULL)
					{
						INT_PTR nAllocCount = __max(nCount, AllocObjectCountOneTime);
						pBlock = (DataBlock*)malloc(sizeof(DataBlock) + sizeof(T) * nAllocCount);
						memset(pBlock, 0, sizeof(DataBlock) + sizeof(T) * nAllocCount);
						pBlock->pFreePtr = (T*)(pBlock + 1);
						pBlock->pEndPtr = pBlock->pFreePtr + nAllocCount;
						pBlock->pPrev = m_pDataBlock;
						m_pDataBlock = pBlock;
					}
				}
				//返回对象指针
				result = pBlock->pFreePtr;
				pBlock->pFreePtr += nCount;
				return result;
			}
			//获取内存块数量
			inline INT_PTR blockCount() const
			{
				INT_PTR result = 0;
				DataBlock *pBlock = m_pDataBlock;

				while (pBlock)
				{
					result++;
					pBlock = pBlock->pPrev;
				}
				return result;
			}
			/*
			* Comments: 枚举内存块描述信息
			* Param LPCVOID lpEnumKey: 上次枚举的返回值，作为枚举下一个的依据，第一次枚举此值必须为NULL。
			* Param DataBlockDesc & desc: 用于保存内存块描述的对象
			* @Return LPCVOID: 返回一个内存块枚举key指针，便于进行下次枚举。如果返回值为NULL则表示枚举结束
			*
			* ★★注意★★：枚举内存块描述的操作是倒叙进行的，第一个被枚举的内存块描述为左后一个内存块的描述！
			*
			* ★★示例代码★★：-------------------------------------------------
			*  LPCVOID lpEnumKey = NULL;
			*  CDBGObjectAllocator<T>::DataBlockDesc desc;
			*  while (lpEnumKey = allocator.enumBlockDesc(lpEnumKey, desc))
			*  {
			*     printf("Memory Block Info { Address = %X, Size = %u }",
			*	    desc.lpBaseAddress, desc.dwBlockSize);
			*  }
			* --------------------------------------------------------------
			*/
			inline LPCVOID enumBlockDesc(LPCVOID lpEnumKey, DataBlockDesc &desc) const
			{
				DataBlock *pBlock = (DataBlock*)lpEnumKey;
				if (!pBlock)
					pBlock = m_pDataBlock;
				else pBlock = pBlock->pPrev;

				if (pBlock)
				{
					desc.lpBaseAddress = ((const char*)pBlock) + sizeof(*pBlock);
					desc.dwBlockSize = pBlock->pEndPtr - desc.lpBaseAddress;
					return pBlock;
				}
				return NULL;
			}
			void clear()
			{
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
			CDBGObjectAllocator()
			{
				m_pDataBlock = NULL;
			}
			~CDBGObjectAllocator()
			{
				clear();
			}
		};

	}
}