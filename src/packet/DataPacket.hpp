#pragma once

/************************************************************************/
/*                        �����ڴ�����ݰ���д��
/*
/*  ʵ�ַ�ʽ��MemoryStream��ͬ��д�����ݵ�ʱ��д�뵽�ڴ��У����д�����ݵĿռ䳬������
/*  �ڴ�鳤������Զ������ڴ档
/*
/*  ���ݰ����ڴ�����ʱͨ���ڴ�ض���CBufferAllocator���еģ���ʹ�ø���Ч�ʵ��ڴ����
/*
/*  �����ݰ��е��ַ���
/*  ���ݰ�����һ���ַ�����д���ƣ�����ANSI�ַ����Լ�UNICODE16�ַ�������һ���ַ���д��
/*  �����ݰ������ڴ�ṹΪ��
/*    [2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ���������65536����][�ַ�����ֹ�ַ�0]
/*
/************************************************************************/
#include <stdlib.h>
#include "DataPacketReader.hpp"

namespace alloctrace
{
	namespace packet
	{

		class CDataPacket : public CDataPacketReader
		{
		public:
			/* д�����������
			* lpBuffer  ָ������������ڴ��ָ��
			* dwSize    ���ݵ��ֽڳ���
			*/
			inline void writeBuf(LPCVOID lpBuffer, size_t dwSize)
			{
				size_t dwMemSize = m_pMemoryEnd - m_pOffset;
				if (dwMemSize < dwSize)
				{
					setSize(m_pMemoryEnd - m_pMemory + dwSize);
				}
				memcpy(m_pOffset, lpBuffer, dwSize);
				m_pOffset += dwSize;
				//�����дָ�볬������ָ���λ�ã����������ָ��Ϊ��дָ���λ��
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
			}
			/* д��ԭ����������
			* bool,char,short,int,__int64,float,double,void*��Щ��������Ϊԭ������
			*/
			template <typename T>
			inline void writeAtom(T data)
			{
				size_t dwMemSize = m_pMemoryEnd - m_pOffset;
				if (dwMemSize < sizeof(data))
				{
					setSize(m_pMemoryEnd - m_pMemory + sizeof(data));
				}
				*(T*)m_pOffset = data;
				m_pOffset += sizeof(data);
				//�����дָ�볬������ָ���λ�ã����������ָ��Ϊ��дָ���λ��
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
			}

			inline void writeString(const char *str, size_t len = -1)
			{
				rawWriteStringLen<unsigned short>(str, len);
			}
			/* д��UNICODE16�ַ������ݣ��ַ������ȱ�����65536���ڣ�����65535�Ĳ��ֽ����ᱻд�룩
			* ���ַ���֮ǰд�������ֽڵ��ַ������ȣ��һ����ַ���ĩβд����ֹ�ַ�
			* len ���ֵΪ-1�����Զ�����str�ĳ���
			*/
			inline void writeWideString(const wchar_t *str, size_t len = -1)
			{
				rawWriteWideStringLen<unsigned short>(str, len);
			}

		public:
			CDataPacket()
			{
				m_boInitFromStaticMem = false;
			}
			/* �������ݰ������ݳ���
			* ������õĳ��ȱȵ�ǰ����С�Ҷ�дƫ��ָ�볬���µĳ��ȣ���Ὣ��дƫ��ָ����������³���һ��
			* ����µĳ��ȱȵ�ǰ�ĳ���Ҫ����Զ���չ�ڴ���С
			*/
			inline void setLength(size_t newLength)
			{
				if (newLength > (size_t)(m_pMemoryEnd - m_pMemory))
					setSize(newLength);
				m_pDataEnd = m_pMemory + newLength;
				//����������ݳ��Ⱥ�ƫ��λ�ó�������λ�ã������ƫ��λ��Ϊ����λ��
				if (m_pOffset > m_pDataEnd)
					m_pOffset = m_pDataEnd;
			}
			/* �������ݰ���ǰ��дλ���ֽ�ƫ������
			* ����µ�ƫ�����ȵ�ǰ�ڴ�鳤��Ҫ����������setLengthһ�������ڴ�����չ����
			*/
			inline size_t setPosition(size_t newPos)
			{
				size_t dwMemSize = m_pMemoryEnd - m_pMemory;
				if (newPos > dwMemSize)
					setSize(newPos);
				m_pOffset = m_pMemory + newPos;
				//�����дָ�볬������ָ���λ�ã����������ָ��Ϊ��дָ���λ��
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
				return newPos;
			}
			/* ���ڵ�ǰ��дָ��ƫ��λ�õ���ƫ����
			* nOffsetToAdjust Ҫ������ƫ������С���������ʾ���ڴ濪ʼ������������ƫ�ƣ�
			* �����ڻ����Ƶ������ƫ��λ�ñ�����ڵ����ڴ濪ͷ��
			* ���������ı��˳��������ڴ泤�ȣ�����Զ������ڴ浽���������λ��
			*/
			inline size_t adjustOffset(INT64 nOffsetToAdjust)
			{
				m_pOffset += nOffsetToAdjust;
				if (m_pOffset < m_pMemory)
					m_pOffset = m_pMemory;
				else if (m_pOffset > m_pMemoryEnd)
					setSize(m_pOffset - m_pMemory);
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
				return m_pOffset - m_pMemory;
			}
			/*
			* ��չ�ڴ滺��������
			*/
			inline size_t reserve(size_t nSize)
			{
				size_t Result = m_pMemoryEnd - m_pMemory;
				if (Result < nSize)
					setSize(nSize);
				return Result;
			}
			/*
			* ��ȡ��ǰ���峤���µĿ����ֽ���
			*/
			inline size_t getAvaliableCapacity()
			{
				return m_pMemoryEnd - m_pOffset;
			}
			/*
			* ��ȡ���ݰ��ܴ�С
			*/
			inline size_t getCapacity()
			{
				size_t Result = m_pMemoryEnd - m_pMemory;
				return Result;
			}
			/*
			* ������ݰ����ͷ��ڴ�
			*/
			inline void clear()
			{
				if (m_pMemory && !m_boInitFromStaticMem)
				{
					free(m_pMemory);
				}
				m_pMemory = m_pMemoryEnd = m_pOffset = m_pDataEnd = NULL;
			}
		public:
			/*
			* Comment: ��������ͨ������Ԥ�����_DPPACK_MEM_CHK_������£������ݰ������ڴ�Խ����
			* @Return void:
			*/
			inline void _Assert()
			{
#ifdef _DPPACK_MEM_CHK_
				DbgAssert(!m_pMemoryEnd || *(PINT)m_pMemoryEnd == 0xCCDDCCEE);
#endif
			}
		private:
			/*
			* ������ø�ֵ
			*/
			inline CDataPacket& operator = (const CDataPacket& an);
			/*{
			//��ֹ���ø�ֵ��
			Assert(false);
			}*/
		public:
			/* д��ԭ�����ݵ���������� */
			template <typename T>
			inline CDataPacket& operator << (T val)
			{
				if (sizeof(val) <= sizeof(INT_PTR))
					writeAtom<T>(val);
				else writeBuf(&val, sizeof(val));
				return *this;
			}
			inline CDataPacket& operator << (const char* val)
			{
				writeString(val, strlen(val));
				return *this;
			}
			inline CDataPacket& operator << (char* val)
			{
				writeString(val, strlen(val));
				return *this;
			}
			inline CDataPacket& operator << (const wchar_t* val)
			{
				writeWideString(val, wcslen(val));
				return *this;
			}
			inline CDataPacket& operator << (wchar_t* val)
			{
				writeWideString(val, wcslen(val));
				return *this;
			}

		private:
			CDataPacket(const CDataPacket &another);
			/*{
			//Ϊ�˷�ֹ�����ø�ֵ��һ�����󣬴˴���ֹ����һ�������й���
			Assert(FALSE);
			}*/
		public:
			typedef CDataPacketReader Inherited;
			CDataPacket(LPCVOID lpDataPtr, size_t dwSize)
				:Inherited(lpDataPtr, dwSize)
			{
				m_pDataEnd = m_pMemory;
				m_boInitFromStaticMem = true;
			}
			~CDataPacket()
			{
				clear();
			}

		protected:
			/*  �����趨���ݰ��ڴ�ռ��С  */
			void setSize(size_t dwNewSize)
			{
				char *pOldMemory = m_pMemory;
				//�ڴ泤�Ȱ�CDataPacket::MemoryAlginmentSize�ֽڶ���
				//���ݳ�������4�ֽڣ���������0��ֹ���Ա�����ַ����Ľ�β����
				dwNewSize = (dwNewSize + MemoryAlginmentSize) & (~(MemoryAlginmentSize - 1));
#ifdef _DPPACK_MEM_CHK_
				char *pMemory = (char*)malloc(dwNewSize + sizeof(INT));
#else
				char *pMemory = (char*)malloc(dwNewSize);
#endif
				size_t dwOffset = m_pOffset - m_pMemory;
				size_t dwLength = m_pDataEnd - m_pMemory;
				if (dwLength > 0)
				{
					memcpy(pMemory, m_pMemory, __min(dwNewSize, dwLength));
				}
				//��������ָ��
				m_pMemory = pMemory;
				m_pMemoryEnd = m_pMemory + dwNewSize;
#ifdef _DPPACK_MEM_CHK_
				*(PINT)m_pMemoryEnd = 0xCCDDCCEE;
#endif
				m_pOffset = m_pMemory + dwOffset;
				m_pDataEnd = m_pMemory + dwLength;
				//����ԭ�������ڴ�
				if (pOldMemory)
				{
					_Assert();
					free(pOldMemory);
				}
			}
			/* д��ANSI���ַ����Լ��ַ�������
			* �ַ��������ݸ�ʽΪ��[2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ���������65536����][�ַ�����ֹ�ַ�0]���ַ����г�������65535��Ĳ��ֽ�������
			* ���ַ���֮ǰд��sizeof(TL)�ֽڵĳ���ֵ���һ����ַ���ĩβд����ֹ�ַ�
			* <TL> �����ַ��������������ݵ��������ͣ����ڶ��ַ���Ӧ��Ϊunsigned char��������unsigned short
			* len	���ֵΪ-1�����Զ�����str�ĳ���
			*/
			template <typename TL>
			void rawWriteStringLen(const char* str, size_t len)
			{
				if (len == -1)
					len = str ? (TL)strlen(str) : 0;
				writeAtom<TL>((TL)len);//д�볤��
				writeBuf(str, len * sizeof(*str));
				writeAtom<char>(0);//д����ֹ�ַ�
			}
			/* д��UNICODE16���ַ����Լ��ַ�������
			* ������rawWriteLengthString��ͬ
			*/
			template <typename TL>
			void rawWriteWideStringLen(const wchar_t* str, size_t len)
			{
				if (len == -1)
					len = str ? (TL)wcslen(str) : 0;
				writeAtom<TL>((TL)len);//д�볤��
				writeBuf(str, len * sizeof(*str));
				writeAtom<wchar_t>(0);//д����ֹ�ַ�
			}

		protected:
			bool m_boInitFromStaticMem;

		private:
			static const size_t MemoryAlginmentSize = 256;	//���ݰ��ڴ泤�ȶ���߽�
		};

	}
}
