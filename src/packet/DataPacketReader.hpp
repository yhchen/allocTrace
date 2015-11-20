#pragma once

/************************************************************************/
/*                        基于内存的数据包读取类
/*
/*  ★数据包中的字符串
/*  数据包含有一种字符串读写机制，包括ANSI字符串以及UNICODE16字符串。将一个字符串写入
/*  到数据包中后其内存结构为：
/*    [2字节字符长度数据][字符串字节数据，字符串长度必须在65536以内][字符串终止字符0]
/*
/************************************************************************/

namespace alloctrace
{
	namespace packet
	{

		class CDataPacketReader
		{
		public:
			/* 读取二进制数据
			* lpBuffer	为数据读取的缓冲区内存子指针
			* dwSize	要读取的字节数
			* @return	返回实际读取的字节数
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
			/* 读取原子数据
			* bool,char,short,int,__int64,float,double,void*这些数据属于原子数据
			* 如果剩余数据的长度小于原子数据大小，则只会进行读取剩余的部分并清空高位比特位！
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
			/* 读取ANSI短字符串数据
			* 字符串的数据格式为：[2字节字符长度数据][字符串字节数据，字符串长度在65536以内][字符串终止字符0]
			* str			 字符串读取缓冲区指针
			* dwCharsToRead 表示str的字符数量，非字节长度，如果参数为0则函数返回数据包中短字符串的长度，且不会读取数据到str中
			* @return       如果参数dwCharsToRead值为0则返回数据包中短字符串的长度，否则返回实际读取的字符串长度
			* ★remaks		 如果dwCharsToRead字符数量比数据包中的字符串长度要大，则会自动在读取后的str缓冲区中增加字符串终止字符。
			*               如果dwCharsToRead字符数量等于数据包中的字符串长度，否则不会在str中添加终止字符。
			*               如果dwCharsToRead字符数量小于数据包中的字符串长度，则仅读取dwCharsToRead指定的字符数量并丢弃（从
			数据包中跳过）没有被读取的部分以便接下来可以正确的从数据包中读取后续的数据。
			*/
			inline size_t readString(OUT char *str, size_t dwCharsToRead)
			{
				return rawReadStringLen<char*, unsigned short>(str, dwCharsToRead);
			}
			/* 读取UNICODE16短字符串数据
			* 字符串的数据格式为：[2字节字符长度数据][字符串字节数据，字符串长度在65536以内][字符串终止字符0]
			* str			 字符串读取缓冲区指针
			* dwCharsToRead 表示str的字符数量，非字节长度，如果参数为0则函数返回数据包中短字符串的长度，且不会读取数据到str中
			* @return       如果参数dwCharsToRead值为0则返回数据包中短字符串的长度，否则返回实际读取的字符串长度
			* ★remaks		 如果dwCharsToRead字符数量比数据包中的字符串长度要大，则会自动在读取后的str缓冲区中增加字符串终止字符。
			*               如果dwCharsToRead字符数量等于数据包中的字符串长度，否则不会在str中添加终止字符。
			*               如果dwCharsToRead字符数量小于数据包中的字符串长度，则仅读取dwCharsToRead指定的字符数量并丢弃（从
			数据包中跳过）没有被读取的部分以便接下来可以正确的从数据包中读取后续的数据。
			*/
			inline size_t readWideString(OUT wchar_t *str, size_t dwCharsToRead)
			{
				return rawReadStringLen<wchar_t*, unsigned short>(str, dwCharsToRead);
			}
			/* 获取获取数据包的当前有效数据长度 */
			inline size_t getLength()
			{
				return m_pDataEnd - m_pMemory;
			}
			/* 获取数据包当前读写位置字节偏移量 */
			inline size_t getPosition()
			{
				return m_pOffset - m_pMemory;
			}
			/* 设置数据包当前读写位置字节偏移量，
			如果新的偏移量比当前内存块长度要大，则限制新的偏移位置不超过当前内存块长度
			*/
			inline size_t setPosition(size_t newPos)
			{
				size_t dwMemSize = m_pMemoryEnd - m_pMemory;
				if (newPos > dwMemSize)
					newPos = dwMemSize;
				m_pOffset = m_pMemory + newPos;
				return newPos;
			}
			/* 获取自当前读写指针开始可以用与读取的剩余字节数 */
			inline size_t getAvaliableLength()
			{
				return m_pDataEnd - m_pOffset;
			}
			/* 基于当前读写指针偏移位置调整偏移量
			* nOffsetToAdjust 要调整的偏移量大小，负数则表示向内存开始处调整（降低偏移）
			* 函数内会限制调整后的偏移位置在有效的内存范围内
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
			/* 获取数据包数据内存指针 */
			inline char* getMemoryPtr()
			{
				return m_pMemory;
			}
			/* 获取数据包数据内存的当前偏移指针 */
			inline char* getOffsetPtr()
			{
				return m_pOffset;
			}

			//获取指定偏移量的指针
			inline char* getPositionPtr(size_t nPos)
			{
				return m_pMemory + nPos;
			}
		public:
			/* 读取原子数据的运算符重载 */
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
			/* 读取ANSI/UNICODE16字符串数据
			* 字符串的数据格式为：[2字节字符长度数据][字符串字节数据，字符串长度在65536以内][字符串终止字符0]
			* <TS>			 定义字符串数据类型，是char*或wchar_t*
			* <TL>			 定义字符串长度描述数据的数据类型，对于短字符串应当为unsigned char，否则即是unsigned short
			* str			 字符串读取缓冲区指针
			* lenType		 参数的数据类型定义了
			* dwCharsToRead 表示str的字符数量，非字节长度，如果参数为0则函数返回数据包中短字符串的长度，且不会读取数据到str中
			* @return       如果参数dwCharsToRead值为0则返回数据包中短字符串的长度，否则返回实际读取的字符串长度
			* ★remaks		 如果dwCharsToRead字符数量比数据包中的字符串长度要大，则会自动在读取后的str缓冲区中增加字符串终止字符。
			*               如果dwCharsToRead字符数量等于数据包中的字符串长度，否则不会在str中添加终止字符。
			*               如果dwCharsToRead字符数量小于数据包中的字符串长度，则仅读取dwCharsToRead指定的字符数量并丢弃（从
			数据包中跳过）没有被读取的部分以便接下来可以正确的从数据包中读取后续的数据。
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
						//跳过字符串长度数据
						m_pOffset += sizeof(TL);
						dwAvaliableSize -= sizeof(TL);
						//计算实际读取长度，避免越界
						if (nReadLen > dwCharsToRead)
							nReadLen = dwCharsToRead;
						if (nReadLen > dwAvaliableSize / sizeof(*str))
							nReadLen = dwAvaliableSize / sizeof(*str);
						readBuf(str, nReadLen * sizeof(*str));
						//如果读取的字符串长度少于数据中指定的字符串长度，则需要调过没有读取的部分，
						//以便接下来可以正确的从数据包中读取后续的数据。
						if (nStrLen > nReadLen)
							m_pOffset += nStrLen - nReadLen;
						//跳过终止字符0
						dwAvaliableSize -= sizeof(*str) * nStrLen;
						if (dwAvaliableSize >= sizeof(*str))
							m_pOffset += sizeof(*str);
					}
				}
				else nReadLen = 0;
				//如果读取缓冲区有剩余的空间则添加终止字符
				if (nReadLen < dwCharsToRead)
					str[nReadLen] = 0;
				//zac：强制补0
				else if (dwCharsToRead > 0)
					str[dwCharsToRead - 1] = 0;
				return nStrLen;
			}
			/* 读取ANSI/UNICODE16字符串数据
			* 字符串的数据格式为：[2字节字符长度数据][字符串字节数据，字符串长度在65536以内][字符串终止字符0]
			* <TS>			 定义字符数据类型，是char或wchar_t
			* <TL>			 定义字符串长度描述数据的数据类型，对于短字符串应当为unsigned char，否则即是unsigned short
			* str			 字符串读取缓冲区指针
			* lenType		 参数的数据类型定义了
			* dwCharsToRead 表示str的字符数量，非字节长度，如果参数为0则函数返回数据包中短字符串的长度，且不会读取数据到str中
			* @return       如果参数dwCharsToRead值为0则返回数据包中短字符串的长度，否则返回实际读取的字符串长度
			* ★remaks		 如果dwCharsToRead字符数量比数据包中的字符串长度要大，则会自动在读取后的str缓冲区中增加字符串终止字符。
			*               如果dwCharsToRead字符数量等于数据包中的字符串长度，否则不会在str中添加终止字符。
			*               如果dwCharsToRead字符数量小于数据包中的字符串长度，则仅读取dwCharsToRead指定的字符数量并丢弃（从
			数据包中跳过）没有被读取的部分以便接下来可以正确的从数据包中读取后续的数据。
			*/
			template <typename TS, typename TL>
			const TS* rawReadStringPtr()
			{
				size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
				size_t nStrLen = 0;
				if (dwAvaliableSize >= sizeof(TL) + sizeof(TS))
				{
					nStrLen = *((TL*)m_pOffset);
					//如果数据包中有足够的字符串数据空间
					if (dwAvaliableSize >= nStrLen + sizeof(TL) + sizeof(TS))
					{
						const TS* str = (TS*)(m_pOffset + sizeof(TL));
						m_pOffset += nStrLen * sizeof(TS) + sizeof(TL) + sizeof(TS);//调过字符串内容
						return str;
					}
				}
				return NULL;
			}
		protected:
			char				*m_pMemory;		//内存起始地址
			char				*m_pMemoryEnd;	//内存结束地址，指向结尾字节的后一个字节
			char				*m_pOffset;		//当前读写偏移地址
			char				*m_pDataEnd;	//数据长度结束偏移地址
		};

	}
}

