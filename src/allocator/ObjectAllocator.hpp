#pragma once

/************************************************************************/
/*                                �����ڴ�������
/*
/*      ��������ڴ�����С�ڴ����������Ż����Ҷ����ǳ�ʱ��ʹ�ö������ͷŵĳ��ϡ�
/*                       ����ڴ���������Ƕ��̰߳�ȫ�ģ���
/*
/*   ���ڶ���������ͬ�����С���ڴ���������Ż���������ֻ�ṩ�������Ĺ��ܲ����ܹ����ڴ�
/* �����Ч�ʽ���������������ṩ�ͷŶ�����ռ���ڴ�Ĺ��ܡ����ж�����ռ�õ��ڴ潫������
/* ������ʱ�ű��ͷš�
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
			//�ڴ�������ṹ�����ڲ�ѯ�ڴ����Ϣ
			struct DataBlockDesc
			{
				const char* lpBaseAddress;//�ڴ��ָ���ַ
				size_t dwBlockSize;		  //�ڴ���С
			};

		private:
			//�ڴ������ڵ�
			struct DataBlock
			{
				T*			pFreePtr;	//ָ����еĶ���
				T*			pEndPtr;	//ָ������ڴ��Ľ���
				DataBlock*	pPrev;
			}		*m_pDataBlock;

		public:
			enum {
				AllocObjectCountOneTime = 1024
			};
			//��������ָ��
			T* allocObjects(const INT_PTR nCount = 1)
			{
				T* result;
				DataBlock *pBlock = m_pDataBlock;
				//���Դӵ�ǰ�ڴ��������nCount�Ķ�������ڴ����û���㹻�Ķ�������֮ǰ���ڴ���в����Ƿ�����㹻�Ŀ��ж���
				if (!pBlock || pBlock->pEndPtr - pBlock->pFreePtr < nCount)
				{
					//��֮ǰ���ڴ���в����Ƿ�����㹻�Ŀ��ж���
					while (pBlock && (pBlock = pBlock->pPrev))
					{
						if (pBlock->pEndPtr - pBlock->pFreePtr >= nCount)
							break;
					}
					//��������ڴ���ж�û���㹻�Ķ������������ڴ��
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
				//���ض���ָ��
				result = pBlock->pFreePtr;
				pBlock->pFreePtr += nCount;
				return result;
			}
			//��ȡ�ڴ������
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
			* Comments: ö���ڴ��������Ϣ
			* Param LPCVOID lpEnumKey: �ϴ�ö�ٵķ���ֵ����Ϊö����һ�������ݣ���һ��ö�ٴ�ֵ����ΪNULL��
			* Param DataBlockDesc & desc: ���ڱ����ڴ�������Ķ���
			* @Return LPCVOID: ����һ���ڴ��ö��keyָ�룬���ڽ����´�ö�١��������ֵΪNULL���ʾö�ٽ���
			*
			* ���ע���ö���ڴ�������Ĳ����ǵ�����еģ���һ����ö�ٵ��ڴ������Ϊ���һ���ڴ���������
			*
			* ���ʾ�������-------------------------------------------------
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