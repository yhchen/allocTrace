#ifndef __WIN32_ALLOC_TRACE_NODE_H__
#define __WIN32_ALLOC_TRACE_NODE_H__

class AllocCallNode
{
public:
	SG2D::String fnName;//������
	size_t totalBytes;//�������ֽ���
	size_t totalBlocks;//���������
	size_t aliveBytes;//��Ծ�ֽ���
	size_t aliveBlocks;//��Ծ����
	void* hTreeNode;//TreeView�ڵ�
	AllocCallNode* parent;//���ڵ�
	AllocCallNode* children;//�ӽڵ�
	AllocCallNode* sibling;//�ڽڵ�

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
