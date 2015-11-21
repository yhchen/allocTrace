#ifndef __ALLOC_TRACE_ALLOCRECODEROUTPUTPIPE_H__
#define __ALLOC_TRACE_ALLOCRECODEROUTPUTPIPE_H__

#include "os/pipe/NamedPipe.h"
#include "os/Thread.h"
#include "os/Lock.h"
#include "packet/HeapAllocDataPacket.hpp"

namespace alloctrace
{
	namespace recoder
	{

		using namespace alloctrace::os;
		using namespace alloctrace::os::pipe;
		using namespace alloctrace::packet;

		class AllocRecordOutputPipe : public CBaseThread, public CNamedPipeClient
		{
		public:
			/*static void* operator new (size_t size)
			{
			return HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size);
			}
			static void operator delete (void* ptr)
			{
			HeapFree(GetProcessHeap(), 0, ptr);
			}*/
			AllocRecordOutputPipe();
			void writePipe(CHeapAllocDataPacket &packet);

		protected:
			void OnRountine();

		private:
			bool connected();
			void sendBuffers();

		private:
			CCSLock m_PacketsLock;
			CHeapAllocDataPacket	m_Packets[2];
			CHeapAllocDataPacket* m_pAppendPacket;
			CHeapAllocDataPacket* m_pWritePacket;
			DWORD64 dwReconnectPipeTime;
		};

	}
}

#endif // !__ALLOC_TRACE_ALLOCRECODEROUTPUTPIPE_H__
