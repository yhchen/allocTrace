#pragma once

/************************************************************************/
/*                          �����ڴ��������������������룩
/*
/*                   ����ö���صļ����Ӵ�����С�ڴ����������Ż���
/*                       ����ڴ���������Ƕ��̰߳�ȫ�ģ���
/*
/*   ���ڶ���������ͬ�����С���ڴ���������Ż���������ֻ�ṩ�������Ĺ��ܲ����ܹ����ڴ�
/* �����Ч�ʽ�������������ṩ�ͷŶ�����ռ���ڴ�Ĺ��ܡ����ж�����ռ�õ��ڴ潫������
/* ������ʱ�ű��ͷš�
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
			//��������ָ��
			T* allocObject()
			{
				T* result;
				INT_PTR nCount = Inherited::count();

				if (nCount <= 0)
				{
					//static const INT_PTR nAllocCount = 4096;
					DataBlock *pBlock = (DataBlock*)malloc(sizeof(DataBlock) + sizeof(T) * AllocObjectCountOneTime);
					memset(pBlock, 0, sizeof(DataBlock) + sizeof(T) * AllocObjectCountOneTime);
					//������ָ��ȫ����ӵ������б���
					result = (T*)(pBlock + 1);
					if (Inherited::maxCount() < AllocObjectCountOneTime)
						reserve(AllocObjectCountOneTime);
					for (INT_PTR i = 0; i < AllocObjectCountOneTime; ++i)
					{
						(*this)[i] = result;
						result++;
					}
					nCount = AllocObjectCountOneTime;
					//������ڴ�����һ���ڴ��
					pBlock->pPrev = m_pDataBlock;
					m_pDataBlock = pBlock;
				}
				//�����б��е����һ������
				nCount--;
				result = (*this)[nCount];
				Inherited::trunc(nCount);
				return result;
			}
			//�ͷ�һ������
			inline void freeObject(T* pObject)
			{
				add(pObject);
			}
			//�ͷŲ�������ж�����ڴ棬��ʹ������������Ķ�����Ч��
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

