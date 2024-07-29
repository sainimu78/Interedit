#pragma once
#include "Niflect/Memory/AllocatorInterface.h"
#include "Niflect/Base/NiflectBase.h"

namespace Niflect
{
	class CPooledMemoryScope : public IAllocatorInterface
	{
	public:
		CPooledMemoryScope()
			: m_poolSize(1024)
			, m_poolMem(NULL)
		{
			//��Ҫ����Ƕ�׵����?
			CMemory::SetCurrentAllocator(this);
		}
		~CPooledMemoryScope()
		{
			CMemory::SetCurrentAllocator(NULL);
			Niflect::CDefaultMemory::Free(m_poolMem);
		}
		virtual void* Alloc(size_t size) override
		{
			m_poolSize += size;
			//todo: ��Ϊ��Ԥ����һ��size
			m_poolMem = Niflect::CDefaultMemory::Realloc(m_poolMem, m_poolSize);
			return m_poolMem;
		}
		virtual void* Realloc(void* ptr, size_t size) override
		{
			return this->Alloc(size);
		}
		virtual void Free(void* ptr) override
		{
		}

	private:
		size_t m_poolSize;
		void* m_poolMem;
	};
}