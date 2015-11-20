//#include "QueueList.h"
#include "alloctrace.h"
#include "os/Thread.h"
#include "os/Lock.h"
#include "stream/BaseStream.h"
#include <crtdbg.h>
#include <map>
#include "packet/DataPacket.hpp"
#include "ObjectAllocator.hpp"
#include "SingleObjectAllocator.hpp"
#include "AppDebug.h"
#include "Stream/FileStream.h"

#pragma warning(disable:4996)

using namespace std;
using namespace std::tr1;
namespace alloctrace
{
	using namespace alloctrace::allocator;
	using namespace alloctrace::container;
	using namespace alloctrace::stream;

	struct AllocRecordSample
	{
		AllocRecord rec;
		size_t* frames;
	};

	class AllocFrame
	{
	public:
		size_t address;//帧地址
		size_t aliveBytes;//活跃内存字节数
		size_t aliveBlocks;//活跃内存块数
		AllocFrame* parent;//父级调用帧
		AllocFrame* children;//子调用帧列表
		AllocFrame* next;//同级下一个调用帧
	public:
		AllocFrame()
		{
		}
		void init(size_t _address)
		{
			address = _address;
			aliveBytes = 0;
			aliveBlocks = 0;
			parent = NULL;
			children = NULL;
			next = NULL;
		}
		AllocFrame* getChild(size_t address)
		{
			AllocFrame* child = children;
			while (child)
			{
				if (child->address == address)
					return child;
				child = child->next;
			}
			return NULL;
		}
		AllocFrame* getDeepChild(size_t address)
		{
			AllocFrame* child = children;
			while (child)
			{
				if (child->address == address)
					return child;
				child = child->next;
			}

			child = children;
			while (child)
			{
				AllocFrame* found = child->getDeepChild(address);
				if (found)
					return found;
				child = child->next;
			}
			return NULL;
		}
	};

	class AllocFunction
	{
	public:
		const char* fnName;//函数名
		size_t aliveBytes;//活跃内存字节数
		size_t aliveBlocks;//活跃内存块数
		AllocFunction* parent;//父函数
		AllocFunction* children;//子函数列表
		AllocFunction* next;//同级下一个函数列表
	public:
		void init(const char* _fnName)
		{
			fnName = _fnName;
			aliveBytes = 0;
			aliveBlocks = 0;
			parent = children = next = NULL;
		}
		AllocFunction* getChild(const char* fnName)
		{
			AllocFunction* child = children;
			while (child)
			{
				if (strcmp(child->fnName, fnName) == 0)
					return child;
				child = child->next;
			}
			return NULL;
		}
	};

	class SimpleAllocTraceFormat
	{
	private:
		typedef std::map<void*, AllocRecordSample> AllocRecordMap;
		typedef pair<void*, AllocRecordSample> AllocRecordPair;

	private:
		CDBGObjectAllocator<size_t> m_AddressAllocator;
		CSingleObjectAllocator<AllocFrame> m_FrameAllocator;
		CSingleObjectAllocator<AllocFunction> m_FunctionAllocator;
		CDBGObjectAllocator<char> m_NameAllocator;
		AllocRecordMap m_AllocRecordMap;
		AllocFrame *m_pRootFrame;
		AllocFunction *m_pRootFunction;

	public:
		SimpleAllocTraceFormat()
		{
			m_pRootFrame = NULL;
			m_pRootFunction = NULL;
		}
	public:
		void dump(const char* fileName, const char* sOutName)
		{
			load(fileName);
			buildFrameTree();
			buildCallTree();
			saveCallTree(sOutName);
		}
	private:
		void clear()
		{
			m_AllocRecordMap.clear();
			m_AddressAllocator.clear();
			m_NameAllocator.clear();
			m_FrameAllocator.freeAll();
			m_FunctionAllocator.freeAll();
			m_pRootFrame = NULL;
			m_pRootFunction = NULL;
		}
		//读取alloctrace文件
		void load(const char* fileName)
		{
			clear();
			AllocRecordMap &recMap = m_AllocRecordMap;

			CFileStream *stm = new CFileStream((LPCTSTR)fileName, CFileStream::faRead | CFileStream::faShareRead | CFileStream::faShareWrite);
			AllocRecordSample recEx;
			size_t dwModeBase;
			stm->read(&dwModeBase, sizeof(dwModeBase));
			dwModeBase = (size_t)GetModuleHandle(NULL) - dwModeBase;

			while (true)
			{
				if (stm->read(&recEx.rec, sizeof(recEx.rec)) != sizeof(recEx.rec))
					break;
				if (recEx.rec.type == atFree)
				{
					AllocRecordMap::iterator it = recMap.find(recEx.rec.ptr);
					if (it != recMap.end())
						recMap.erase(it);
				}
				else if (recEx.rec.type == atAlloc)
				{
					if (recEx.rec.numFrames > 0)
					{
						recEx.frames = m_AddressAllocator.allocObjects(recEx.rec.numFrames);
						const int sizeToRead = sizeof(recEx.frames[0]) * recEx.rec.numFrames;
						if (stm->read(recEx.frames, sizeToRead) != sizeToRead)
							break;
						for (int i = recEx.rec.numFrames - 1; i > -1; --i)
						{
							recEx.frames[i] += dwModeBase;
						}
					}
					recMap.insert(AllocRecordPair(recEx.rec.ptr, recEx));
				}
			}
			delete stm;
		}
		//构建调用帧树
		AllocFrame* buildFrameTree()
		{
			AllocFrame* pRootFrame = m_FrameAllocator.allocObject();
			pRootFrame->init(0);

			AllocRecordMap &recMap = m_AllocRecordMap;
			AllocRecordMap::iterator itEnd = recMap.end();
			for (AllocRecordMap::iterator it = recMap.begin(); it != itEnd; ++it)
			{
				const AllocRecordSample &recEx = it->second;
				if (!recEx.rec.ptr)
					continue;
				AllocFrame *frame;
				if (recEx.rec.numFrames > 0)
				{
					AllocFrame *parentFrame = pRootFrame->getDeepChild(recEx.frames[recEx.rec.numFrames - 1]);
					if (!parentFrame)
					{
						parentFrame = m_FrameAllocator.allocObject();
						parentFrame->init(recEx.frames[recEx.rec.numFrames - 1]);
						parentFrame->parent = pRootFrame;
						parentFrame->next = pRootFrame->children;
						pRootFrame->children = parentFrame;

					}
					for (int i = recEx.rec.numFrames - 2; i > -1; --i)
					{
						frame = parentFrame->getChild(recEx.frames[i]);
						if (!frame)
						{
							frame = m_FrameAllocator.allocObject();
							frame->init(recEx.frames[i]);
							frame->parent = parentFrame;
							frame->next = parentFrame->children;
							parentFrame->children = frame;
						}
						parentFrame = frame;
					}
					frame->aliveBytes += recEx.rec.size;
					frame->aliveBlocks++;
				}
				else
				{
					DebugBreak();
				}
			}
			m_pRootFrame = pRootFrame;
			return pRootFrame;
		}
		//构建调用函数树
		void buildCallTree()
		{
			m_pRootFunction = m_FunctionAllocator.allocObject();
			m_pRootFunction->init("");

			makeCallTree(m_pRootFunction, m_pRootFrame);
		}
		void makeCallTree(AllocFunction *parentFunction, AllocFrame *parentFrame)
		{
			if (parentFrame->children)
			{
				CodeAddressLineInfo li;
				char sNameBuf[32];
				const char *namePtr;
				for (AllocFrame *frame = parentFrame->children; frame != NULL; frame = frame->next)
				{
					AllocFunction* pFunction;
					if (!li.getAddressInfo(frame->address))
					{
						sprintf(sNameBuf, "%llX", frame->address);
						namePtr = sNameBuf;
					}
					else namePtr = li.fnName;

					if (strcmp(namePtr, parentFunction->fnName) == 0)
					{
						makeCallTree(parentFunction, frame);
					}
					else
					{
						pFunction = parentFunction->getChild(namePtr);
						if (!pFunction)
						{
							char* sFnName = m_NameAllocator.allocObjects(strlen(namePtr) + 1);
							strcpy(sFnName, namePtr);

							pFunction = m_FunctionAllocator.allocObject();
							pFunction->init(sFnName);
							pFunction->parent = parentFunction;
							pFunction->next = pFunction->parent->children;
							pFunction->parent->children = pFunction;
						}
						makeCallTree(pFunction, frame);
					}
				}
			}
			else
			{
				const size_t recSize = parentFrame->aliveBytes;
				const size_t recCount = parentFrame->aliveBlocks;
				while (parentFunction)
				{
					parentFunction->aliveBytes += recSize;
					parentFunction->aliveBlocks += recCount;
					parentFunction = parentFunction->parent;
				}
			}
		}
		//保存调用函数树到XML中
		void saveCallTree(const char* fileName)
		{
			char sBuffer[2048], sTransBuffer[1024];
			const char sXmlHeader[] = "<?xml version=\"1.0\"?>";

			CFileStream *pFile = new CFileStream((LPCTSTR)fileName, CFileStream::faCreate);
			pFile->write(sXmlHeader, sizeof(sXmlHeader) - 1);
			writeCallTreeToFile(pFile, m_pRootFunction, sBuffer, sTransBuffer);
			delete pFile;
		}
		void writeCallTreeToFile(CBaseStream *stream, AllocFunction *pFunction, char* tempBuffer, char* transBuffer)
		{
			transFunctionName(transBuffer, pFunction->fnName);
			int nLen = sprintf(tempBuffer, "<F name=\"%s\" aliveBytes=\"%llu\" aliveBlocks=\"%llu\">",
				transBuffer, (unsigned long long)pFunction->aliveBytes, (unsigned long long)pFunction->aliveBlocks);
			stream->write(tempBuffer, nLen);
			for (AllocFunction *pChild = pFunction->children; pChild != NULL; pChild = pChild->next)
			{
				writeCallTreeToFile(stream, pChild, tempBuffer, transBuffer);
			}
			stream->write("</F>", 4);
		}
		void transFunctionName(char *buffer, const char* fnName)
		{
			while (fnName[0])
			{
				int ch = fnName[0];
				fnName++;
				if (ch == '<')
				{
					memcpy(buffer, "&lt;", 4);
					buffer += 4;
				}
				else if (ch == '>')
				{
					memcpy(buffer, "&gt;", 4);
					buffer += 4;
				}
				else
				{
					buffer[0] = ch;
					buffer++;
				}
			}
			buffer[0] = 0;
		}
	};

	void SimpleDumpAllocTrace(const char* fileName)
	{
		SimpleAllocTraceFormat dumper;

		char *sOutName = (char*)malloc(strlen(fileName) + 32);
		strcpy(sOutName, fileName);
		char *sExt = strrchr(sOutName, '.');
		if (!sExt) sExt = sOutName + strlen(sOutName);
		strcpy(sExt, ".stacktrace.xml");

		dumper.dump(fileName, sOutName);

		free(sOutName);
	}
}

