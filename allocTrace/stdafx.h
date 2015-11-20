// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"
///以下下为标准库的头文件
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


///以下为工程里的库头文件
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

// TODO: 在此处引用程序需要的其他头文件
#include "resource.h"
#include "treelist/TreeList.h"
#include "monitor/AllocTraceMonitor.h"
#include "MainWnd.h"

#define FONTNAME	"宋体"//"Courier New"
#define FONTSIZE	12
