#ifndef __ALLOC_TRACE_HANDLESTREAM_H__
#define __ALLOC_TRACE_HANDLESTREAM_H__

#include "BaseStream.h"

namespace alloctrace
{
	namespace stream
	{
		/*	句柄流，
		流操作的数据是针对外部提供的句柄进行的
		*/
#if defined(WIN32)
		class CHandleStream
			: public CBaseStream
		{
		public:
			typedef CBaseStream Inherited;
		protected:
			HANDLE	m_hHandle;
			void setHandle(HANDLE Handle);
		public:
			CHandleStream(HANDLE StreamHandle);
			inline HANDLE getHandle();
			inline bool handleValid();

			bool setSize(size_t tSize);
			size_t seek(const size_t tOffset, const int Origin);
			size_t read(LPVOID lpBuffer, const size_t tSizeToRead);
			size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite);
		};
#else
		class CHandleStream : public CBaseStream
		{
		public:
			typedef CBaseStream Inherited;
		protected:
			FILE*	m_hHandle;
			void setHandle(FILE* Handle);
		public:
			CHandleStream(FILE* StreamHandle);
			inline const FILE* getHandle(){ return m_hHandle; }
			inline bool handleValid(){ return m_hHandle != 0; }
			bool setSize(size_t tSize);
			size_t seek(const size_t tOffset, const int Origin);
			size_t read(LPVOID lpBuffer, const size_t tSizeToRead);
			size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite);
		};
#endif
	}
}

#endif // __ALLOC_TRACE_HANDLESTREAM_H__
