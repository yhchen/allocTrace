// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
///������Ϊ��׼���ͷ�ļ�
#include <stdio.h>
#include <algorithm>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <locale.h>
#include <time.h>
#include <string.h>
#include <queue>
#include <signal.h>


///����Ϊ������Ŀ�ͷ�ļ�
#include <Platform.h>
#include <_ast.h>
#include <_memchk.h>

#include "os/Thread.h"
#include "stream/BaseStream.h"
#include "os/Lock.h"
#include "wnd/ClassedWnd.h"
#include "os/pipe/NamedPipe.h"

#include "base/Util.h"
#include "base/Atom.h"
#include "base/Object.h"
#include "base/Array.h"

#include "string/SG2DStringDef.h"

#include "mem/MemoryBlock.hpp"
#include "mem/StringHashMap.hpp"
#include "mem/SmartObject.hpp"
#include "mem/ObjectArray.hpp"

#include "seh/SEH.h"

using namespace alloctrace;
using namespace std;
using namespace SG2D;

#include <commctrl.h>

#include "allocator/ObjectAllocator.hpp"
#include "allocator/SingleObjectAllocator.hpp"
#include "packet/DataPacket.hpp"
#include "packet/DataPacketReader.hpp"
#include "AppDebug.h"

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "resource.h"
#include "treelist/TreeList.h"
#include "monitor/AllocTraceMonitor.h"
#include "MainWnd.h"

#define FONTNAME	"����"//"Courier New"
#define FONTSIZE	12
