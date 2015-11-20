#ifndef __WIN32_ALLOC_TRACE_NODE_H__
#define __WIN32_ALLOC_TRACE_NODE_H__

class AllocCallNode
{
public:
	SG2D::String fnName;//函数名
	size_t totalBytes;//总申请字节数
	size_t totalBlocks;//总申请次数
	size_t aliveBytes;//活跃字节数
	size_t aliveBlocks;//活跃块数
	void* hTreeNode;//TreeView节点
	AllocCallNode* parent;//父节点
	AllocCallNode* children;//子节点
	AllocCallNode* sibling;//邻节点

public:
	inline void init(const SG2D::String& _fnName)
	{
		fnName = _fnName;
		totalBytes = totalBlocks = aliveBytes = aliveBlocks = 0;
		hTreeNode = NULL;
		parent = children = sibling = NULL;
	}
	inline void uninit()
	{
		fnName = NULL;
		AllocCallNode* child = children;
		while (child)
		{
			AllocCallNode* next = child->sibling;
			child->uninit();
			child = next;
		}
		parent = children = sibling = NULL;
	}
	inline AllocCallNode* getChild(const SG2D::String& _fnName)
	{
		AllocCallNode* child = children;
		while (child)
		{
			if (child->fnName == _fnName)
				return child;
			child = child->sibling;
		}
		return NULL;
	}
};

struct AllocBlockRecord
{
	size_t blockSize;
	AllocCallNode* node;
};

#endif
