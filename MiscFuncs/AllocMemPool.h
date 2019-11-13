#pragma once

/*
source code from:
https://blog.csdn.net/dustpg/article/details/38756241
*/
#pragma warning ( disable : 4200 )
#include <stdint.h>

class CAllocMemPool
{
	// �ڵ�
	struct Node {
		// ��ڵ�
		Node*               next = nullptr;
		// �ѷ�������
		size_t              allocated = 0;
		// �ϴη���λ��
		uint8_t*               last_allocated = nullptr;
		// ������
		uint8_t                buffer[0];
	};

public:
	CAllocMemPool(size_t _PoolSize);
	~CAllocMemPool();

	// �����ڴ�
	void*               Alloc(size_t size, uint32_t align = sizeof(size_t));
	// �ͷ��ڴ�
	void                Free(void* address);

private:

	// ����ڵ�
	__forceinline Node* new_Node();
	// �׽ڵ�
	Node*	m_pFirstNode;

	size_t	m_EachPoolSize;
};

