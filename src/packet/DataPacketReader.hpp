#pragma once

/************************************************************************/
/*                        �����ڴ�����ݰ���ȡ��
/*
/*  �����ݰ��е��ַ���
/*  ���ݰ�����һ���ַ�����д���ƣ�����ANSI�ַ����Լ�UNICODE16�ַ�������һ���ַ���д��
/*  �����ݰ��к����ڴ�ṹΪ��
/*    [2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ������ȱ�����65536����][�ַ�����ֹ�ַ�0]
/*
/************************************************************************/

namespace alloctrace
{
	namespace packet
	{

		class CDataPacketReader
		{
		public:
			/* ��ȡ����������
			* lpBuffer	Ϊ���ݶ�ȡ�Ļ������ڴ���ָ��
			* dwSize	Ҫ��ȡ���ֽ���
			* @return	����ʵ�ʶ�ȡ���ֽ���
			*/
			inline size_t readBuf(OUT LPVOID lpBuffer, size_t dwSize)
			{
				size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
				if (dwSize > dwAvaliableSize)
					dwSize = dwAvaliableSize;
				if (dwSize > 0)
				{
					memcpy(lpBuffer, m_pOffset, dwSize);
					m_pOffset += dwSize;
				}
				return dwSize;
			}
			/* ��ȡԭ������
			* bool,char,short,int,__int64,float,double,void*��Щ��������ԭ������
			* ���ʣ�����ݵĳ���С��ԭ�����ݴ�С����ֻ����ж�ȡʣ��Ĳ��ֲ���ո�λ����λ��
			*/
			template <typename T>
			inline T readAtom()
			{
				T val;
				size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
				if (dwAvaliableSize >= sizeof(T))
				{
					val = *(T*)m_pOffset;
					m_pOffset += sizeof(T);
				}
				else if (dwAvaliableSize > 0)
				{
					memset(&val, 0, sizeof(val));
					memcpy(&val, m_pOffset, dwAvaliableSize);
					m_pOffset += dwAvaliableSize;
				}
				else memset(&val, 0, sizeof(val));
				return val;
			}
			/* ��ȡANSI���ַ�������
			* �ַ��������ݸ�ʽΪ��[2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ���������65536����][�ַ�����ֹ�ַ�0]
			* str			 �ַ�����ȡ������ָ��
			* dwCharsToRead ��ʾstr���ַ����������ֽڳ��ȣ��������Ϊ0�����������ݰ��ж��ַ����ĳ��ȣ��Ҳ����ȡ���ݵ�str��
			* @return       �������dwCharsToReadֵΪ0�򷵻����ݰ��ж��ַ����ĳ��ȣ����򷵻�ʵ�ʶ�ȡ���ַ�������
			* ��remaks		 ���dwCharsToRead�ַ����������ݰ��е��ַ�������Ҫ������Զ��ڶ�ȡ���str�������������ַ�����ֹ�ַ���
			*               ���dwCharsToRead�ַ������������ݰ��е��ַ������ȣ����򲻻���str�������ֹ�ַ���
			*               ���dwCharsToRead�ַ�����С�����ݰ��е��ַ������ȣ������ȡdwCharsToReadָ�����ַ���������������
			���ݰ���������û�б���ȡ�Ĳ����Ա������������ȷ�Ĵ����ݰ��ж�ȡ���������ݡ�
			*/
			inline size_t readString(OUT char *str, size_t dwCharsToRead)
			{
				return rawReadStringLen<char*, unsigned short>(str, dwCharsToRead);
			}
			/* ��ȡUNICODE16���ַ�������
			* �ַ��������ݸ�ʽΪ��[2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ���������65536����][�ַ�����ֹ�ַ�0]
			* str			 �ַ�����ȡ������ָ��
			* dwCharsToRead ��ʾstr���ַ����������ֽڳ��ȣ��������Ϊ0�����������ݰ��ж��ַ����ĳ��ȣ��Ҳ����ȡ���ݵ�str��
			* @return       �������dwCharsToReadֵΪ0�򷵻����ݰ��ж��ַ����ĳ��ȣ����򷵻�ʵ�ʶ�ȡ���ַ�������
			* ��remaks		 ���dwCharsToRead�ַ����������ݰ��е��ַ�������Ҫ������Զ��ڶ�ȡ���str�������������ַ�����ֹ�ַ���
			*               ���dwCharsToRead�ַ������������ݰ��е��ַ������ȣ����򲻻���str�������ֹ�ַ���
			*               ���dwCharsToRead�ַ�����С�����ݰ��е��ַ������ȣ������ȡdwCharsToReadָ�����ַ���������������
			���ݰ���������û�б���ȡ�Ĳ����Ա������������ȷ�Ĵ����ݰ��ж�ȡ���������ݡ�
			*/
			inline size_t readWideString(OUT wchar_t *str, size_t dwCharsToRead)
			{
				return rawReadStringLen<wchar_t*, unsigned short>(str, dwCharsToRead);
			}
			/* ��ȡ��ȡ���ݰ��ĵ�ǰ��Ч���ݳ��� */
			inline size_t getLength()
			{
				return m_pDataEnd - m_pMemory;
			}
			/* ��ȡ���ݰ���ǰ��дλ���ֽ�ƫ���� */
			inline size_t getPosition()
			{
				return m_pOffset - m_pMemory;
			}
			/* �������ݰ���ǰ��дλ���ֽ�ƫ������
			����µ�ƫ�����ȵ�ǰ�ڴ�鳤��Ҫ���������µ�ƫ��λ�ò�������ǰ�ڴ�鳤��
			*/
			inline size_t setPosition(size_t newPos)
			{
				size_t dwMemSize = m_pMemoryEnd - m_pMemory;
				if (newPos > dwMemSize)
					newPos = dwMemSize;
				m_pOffset = m_pMemory + newPos;
				return newPos;
			}
			/* ��ȡ�Ե�ǰ��дָ�뿪ʼ���������ȡ��ʣ���ֽ��� */
			inline size_t getAvaliableLength()
			{
				return m_pDataEnd - m_pOffset;
			}
			/* ���ڵ�ǰ��дָ��ƫ��λ�õ���ƫ����
			* nOffsetToAdjust Ҫ������ƫ������С���������ʾ���ڴ濪ʼ������������ƫ�ƣ�
			* �����ڻ����Ƶ������ƫ��λ������Ч���ڴ淶Χ��
			*/
			inline size_t adjustOffset(INT64 nOffsetToAdjust)
			{
				m_pOffset += nOffsetToAdjust;
				if (m_pOffset < m_pMemory)
					m_pOffset = m_pMemory;
				else if (m_pOffset > m_pMemoryEnd)
					m_pOffset = m_pMemoryEnd;
				return m_pOffset - m_pMemory;
			}
			/* ��ȡ���ݰ������ڴ�ָ�� */
			inline char* getMemoryPtr()
			{
				return m_pMemory;
			}
			/* ��ȡ���ݰ������ڴ�ĵ�ǰƫ��ָ�� */
			inline char* getOffsetPtr()
			{
				return m_pOffset;
			}

			//��ȡָ��ƫ������ָ��
			inline char* getPositionPtr(size_t nPos)
			{
				return m_pMemory + nPos;
			}
		public:
			/* ��ȡԭ�����ݵ���������� */
			template <typename T>
			inline CDataPacketReader& operator >> (T& val)
			{
				if (sizeof(val) <= sizeof(INT_PTR))
					val = readAtom<T>();
				else readBuf(&val, sizeof(val));
				return *this;
			}
			inline CDataPacketReader& operator >> (const char* &str)
			{
				str = rawReadStringPtr<char, unsigned short>();
				return *this;
			}
			inline CDataPacketReader& operator >> (const wchar_t* &str)
			{
				str = rawReadStringPtr<wchar_t, unsigned short>();
				return *this;
			}
		public:
			CDataPacketReader()
			{
				m_pMemory = m_pMemoryEnd = m_pOffset = m_pDataEnd = NULL;
			}
			CDataPacketReader(LPCVOID lpDataPtr, size_t dwSize)
			{
				m_pOffset = m_pMemory = (char*)lpDataPtr;
				m_pMemoryEnd = m_pDataEnd = m_pMemory + dwSize;
			}

		protected:
			/* ��ȡANSI/UNICODE16�ַ�������
			* �ַ��������ݸ�ʽΪ��[2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ���������65536����][�ַ�����ֹ�ַ�0]
			* <TS>			 �����ַ����������ͣ���char*��wchar_t*
			* <TL>			 �����ַ��������������ݵ��������ͣ����ڶ��ַ���Ӧ��Ϊunsigned char��������unsigned short
			* str			 �ַ�����ȡ������ָ��
			* lenType		 �������������Ͷ�����
			* dwCharsToRead ��ʾstr���ַ����������ֽڳ��ȣ��������Ϊ0�����������ݰ��ж��ַ����ĳ��ȣ��Ҳ����ȡ���ݵ�str��
			* @return       �������dwCharsToReadֵΪ0�򷵻����ݰ��ж��ַ����ĳ��ȣ����򷵻�ʵ�ʶ�ȡ���ַ�������
			* ��remaks		 ���dwCharsToRead�ַ����������ݰ��е��ַ�������Ҫ������Զ��ڶ�ȡ���str�������������ַ�����ֹ�ַ���
			*               ���dwCharsToRead�ַ������������ݰ��е��ַ������ȣ����򲻻���str�������ֹ�ַ���
			*               ���dwCharsToRead�ַ�����С�����ݰ��е��ַ������ȣ������ȡdwCharsToReadָ�����ַ���������������
			���ݰ���������û�б���ȡ�Ĳ����Ա������������ȷ�Ĵ����ݰ��ж�ȡ���������ݡ�
			*/
			template <typename TS, typename TL>
			size_t rawReadStringLen(OUT TS str, size_t dwCharsToRead)
			{
				size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
				size_t nReadLen = 0, nStrLen = 0;
				if (dwAvaliableSize > 0)
				{
					TL* temp = (TL*)m_pOffset;
					nReadLen = nStrLen = *temp;
					//nReadLen = nStrLen = (TL)*m_pOffset;

					if (dwCharsToRead > 0)
					{
						//�����ַ�����������
						m_pOffset += sizeof(TL);
						dwAvaliableSize -= sizeof(TL);
						//����ʵ�ʶ�ȡ���ȣ�����Խ��
						if (nReadLen > dwCharsToRead)
							nReadLen = dwCharsToRead;
						if (nReadLen > dwAvaliableSize / sizeof(*str))
							nReadLen = dwAvaliableSize / sizeof(*str);
						readBuf(str, nReadLen * sizeof(*str));
						//�����ȡ���ַ�����������������ָ�����ַ������ȣ�����Ҫ����û�ж�ȡ�Ĳ��֣�
						//�Ա������������ȷ�Ĵ����ݰ��ж�ȡ���������ݡ�
						if (nStrLen > nReadLen)
							m_pOffset += nStrLen - nReadLen;
						//������ֹ�ַ�0
						dwAvaliableSize -= sizeof(*str) * nStrLen;
						if (dwAvaliableSize >= sizeof(*str))
							m_pOffset += sizeof(*str);
					}
				}
				else nReadLen = 0;
				//�����ȡ��������ʣ��Ŀռ��������ֹ�ַ�
				if (nReadLen < dwCharsToRead)
					str[nReadLen] = 0;
				//zac��ǿ�Ʋ�0
				else if (dwCharsToRead > 0)
					str[dwCharsToRead - 1] = 0;
				return nStrLen;
			}
			/* ��ȡANSI/UNICODE16�ַ�������
			* �ַ��������ݸ�ʽΪ��[2�ֽ��ַ���������][�ַ����ֽ����ݣ��ַ���������65536����][�ַ�����ֹ�ַ�0]
			* <TS>			 �����ַ��������ͣ���char��wchar_t
			* <TL>			 �����ַ��������������ݵ��������ͣ����ڶ��ַ���Ӧ��Ϊunsigned char��������unsigned short
			* str			 �ַ�����ȡ������ָ��
			* lenType		 �������������Ͷ�����
			* dwCharsToRead ��ʾstr���ַ����������ֽڳ��ȣ��������Ϊ0�����������ݰ��ж��ַ����ĳ��ȣ��Ҳ����ȡ���ݵ�str��
			* @return       �������dwCharsToReadֵΪ0�򷵻����ݰ��ж��ַ����ĳ��ȣ����򷵻�ʵ�ʶ�ȡ���ַ�������
			* ��remaks		 ���dwCharsToRead�ַ����������ݰ��е��ַ�������Ҫ������Զ��ڶ�ȡ���str�������������ַ�����ֹ�ַ���
			*               ���dwCharsToRead�ַ������������ݰ��е��ַ������ȣ����򲻻���str�������ֹ�ַ���
			*               ���dwCharsToRead�ַ�����С�����ݰ��е��ַ������ȣ������ȡdwCharsToReadָ�����ַ���������������
			���ݰ���������û�б���ȡ�Ĳ����Ա������������ȷ�Ĵ����ݰ��ж�ȡ���������ݡ�
			*/
			template <typename TS, typename TL>
			const TS* rawReadStringPtr()
			{
				size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
				size_t nStrLen = 0;
				if (dwAvaliableSize >= sizeof(TL) + sizeof(TS))
				{
					nStrLen = *((TL*)m_pOffset);
					//������ݰ������㹻���ַ������ݿռ�
					if (dwAvaliableSize >= nStrLen + sizeof(TL) + sizeof(TS))
					{
						const TS* str = (TS*)(m_pOffset + sizeof(TL));
						m_pOffset += nStrLen * sizeof(TS) + sizeof(TL) + sizeof(TS);//�����ַ�������
						return str;
					}
				}
				return NULL;
			}
		protected:
			char				*m_pMemory;		//�ڴ���ʼ��ַ
			char				*m_pMemoryEnd;	//�ڴ������ַ��ָ���β�ֽڵĺ�һ���ֽ�
			char				*m_pOffset;		//��ǰ��дƫ�Ƶ�ַ
			char				*m_pDataEnd;	//���ݳ��Ƚ���ƫ�Ƶ�ַ
		};

	}
}

