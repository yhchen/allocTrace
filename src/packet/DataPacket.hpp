#pragma once

/************************************************************************/
/*                        基于内存的数据包读写类
/*
/*  实现方式与MemoryStream相同，写入数据的时候写入到内存中，如果写入数据的空间超出现有
/*  内存块长度则会自动增长内存。
/*
/*  数据包的内存申请时通过内存池对象CBufferAllocator进行的，以使用更有效率的内存管理。
/*
/*  ★数据包中的字符串
/*  数据包含有一种字符串读写机制，包括ANSI字符串以及UNICODE16字符串。将一个字符串写入
/*  到数据包后其内存结构为：
/*    [2字节字符长度数据][字符串字节数据，字符串长度在65536以内][字符串终止字符0]
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
			/* 写入二进制数据
			* lpBuffer  指向二进制数据内存的指针
			* dwSize    数据的字节长度
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
				//如果读写指针超过长度指针的位置，则调整长度指针为读写指针的位置
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
			}
			/* 写入原子类型数据
			* bool,char,short,int,__int64,float,double,void*这些数据类型为原子类型
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
				//如果读写指针超过长度指针的位置，则调整长度指针为读写指针的位置
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
			}

			inline void writeString(const char *str, size_t len = -1)
			{
				rawWriteStringLen<unsigned short>(str, len);
			}
			/* 写入UNICODE16字符串数据（字符串长度必须在65536以内，超过65535的部分将不会被写入）
			* 在字符串之前写入两个字节的字符串长度，且会再字符串末尾写入终止字符
			* len 如果值为-1，则自动计算str的长度
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
			/* 设置数据包的数据长度
			* 如果设置的长度比当前长度小且读写偏移指针超过新的长度，则会将读写偏移指针调整到与新长度一致
			* 如果新的长度比当前的长度要则会自动扩展内存块大小
			*/
			inline void setLength(size_t newLength)
			{
				if (newLength > (size_t)(m_pMemoryEnd - m_pMemory))
					setSize(newLength);
				m_pDataEnd = m_pMemory + newLength;
				//如果调整数据长度后偏移位置超出长度位置，则调整偏移位置为长度位置
				if (m_pOffset > m_pDataEnd)
					m_pOffset = m_pDataEnd;
			}
			/* 设置数据包当前读写位置字节偏移量，
			* 如果新的偏移量比当前内存块长度要大，则函数会与setLength一样进行内存块的扩展操作
			*/
			inline size_t setPosition(size_t newPos)
			{
				size_t dwMemSize = m_pMemoryEnd - m_pMemory;
				if (newPos > dwMemSize)
					setSize(newPos);
				m_pOffset = m_pMemory + newPos;
				//如果读写指针超过长度指针的位置，则调整长度指针为读写指针的位置
				if (m_pOffset > m_pDataEnd)
					m_pDataEnd = m_pOffset;
				return newPos;
			}
			/* 基于当前读写指针偏移位置调整偏移量
			* nOffsetToAdjust 要调整的偏移量大小，负数则表示向内存开始处调整（降低偏移）
			* 函数内会限制调整后的偏移位置必须大于等于内存开头，
			* 如果调整后的便宜超过现有内存长度，则会自动增长内存到欲求调整的位置
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
			* 扩展内存缓存区长度
			*/
			inline size_t reserve(size_t nSize)
			{
				size_t Result = m_pMemoryEnd - m_pMemory;
				if (Result < nSize)
					setSize(nSize);
				return Result;
			}
			/*
			* 获取当前缓冲长度下的可用字节数
			*/
			inline size_t getAvaliableCapacity()
			{
				return m_pMemoryEnd - m_pOffset;
			}
			/*
			* 获取数据包总大小
			*/
			inline size_t getCapacity()
			{
				size_t Result = m_pMemoryEnd - m_pMemory;
				return Result;
			}
			/*
			* 清空数据包并释放内存
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
			* Comment: 适用于在通过开启预定义宏_DPPACK_MEM_CHK_的情况下，对数据包进行内存越界检查
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
			* 检查引用赋值
			*/
			inline CDataPacket& operator = (const CDataPacket& an);
			/*{
			//禁止引用赋值！
			Assert(false);
			}*/
		public:
			/* 写入原子数据的运算符重载 */
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
			//为了防止将引用赋值给一个对象，此处禁止从另一个对象中构造
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
			/*  重新设定数据包内存空间大小  */
			void setSize(size_t dwNewSize)
			{
				char *pOldMemory = m_pMemory;
				//内存长度按CDataPacket::MemoryAlginmentSize字节对齐
				//数据长度增加4字节，用于增加0终止，以便进行字符串的结尾调试
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
				//重设数据指针
				m_pMemory = pMemory;
				m_pMemoryEnd = m_pMemory + dwNewSize;
#ifdef _DPPACK_MEM_CHK_
				*(PINT)m_pMemoryEnd = 0xCCDDCCEE;
#endif
				m_pOffset = m_pMemory + dwOffset;
				m_pDataEnd = m_pMemory + dwLength;
				//销毁原有数据内存
				if (pOldMemory)
				{
					_Assert();
					free(pOldMemory);
				}
			}
			/* 写入ANSI短字符串以及字符串数据
			* 字符串的数据格式为：[2字节字符长度数据][字符串字节数据，字符串长度在65536以内][字符串终止字符0]，字符串中超过索引65535后的部分将被忽略
			* 在字符串之前写入sizeof(TL)字节的长度值，且会再字符串末尾写入终止字符
			* <TL> 定义字符串长度描述数据的数据类型，对于短字符串应当为unsigned char，否则即是unsigned short
			* len	如果值为-1，则自动计算str的长度
			*/
			template <typename TL>
			void rawWriteStringLen(const char* str, size_t len)
			{
				if (len == -1)
					len = str ? (TL)strlen(str) : 0;
				writeAtom<TL>((TL)len);//写入长度
				writeBuf(str, len * sizeof(*str));
				writeAtom<char>(0);//写入终止字符
			}
			/* 写入UNICODE16短字符串以及字符串数据
			* 规则与rawWriteLengthString相同
			*/
			template <typename TL>
			void rawWriteWideStringLen(const wchar_t* str, size_t len)
			{
				if (len == -1)
					len = str ? (TL)wcslen(str) : 0;
				writeAtom<TL>((TL)len);//写入长度
				writeBuf(str, len * sizeof(*str));
				writeAtom<wchar_t>(0);//写入终止字符
			}

		protected:
			bool m_boInitFromStaticMem;

		private:
			static const size_t MemoryAlginmentSize = 256;	//数据包内存长度对齐边界
		};

	}
}
