#pragma once

template <typename OBJ_TYPE>
class TPageAllocator 
{
	struct node_t
	{
		node_t* next = nullptr; 
	}; 

public:
	OBJ_TYPE* Create() 
	{
		node_t* head = m_freeList; 
		if (head == nullptr) 
		{
			head = (node_t*) CreateBlock(); 
		} 
		else 
		{
			m_freeList = m_freeList->next; 
		}

		OBJ_TYPE* obj = new (head) OBJ_TYPE(); 
		return obj; 
	}

	void Destroy(OBJ_TYPE* obj) 
	{
		obj->~OBJ_TYPE(); 
		node_t* iter = (node_t*) obj; 

		iter->next = m_freeList; 
		m_freeList = iter; 
	}

	void Clear()
	{
		// freeing memory
		if (m_freeList != nullptr)
		{
			node_t* next = m_freeList->next;
			free(m_freeList);

			while (next != nullptr)
			{
				node_t* temp = next->next;
				free(next);
				next = temp;
			}
		}
	}

	OBJ_TYPE* CreateBlock()
	{
		return (OBJ_TYPE*) malloc( sizeof(OBJ_TYPE) ); 
	}

public:
	node_t* m_freeList = nullptr;
}; 