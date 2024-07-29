#include "Niflect/Memory/DefaultMemory.h"
#include "Niflect/Memory/Generic/GenericMemory.h"
#include "Niflect/Memory/Generic/GenericSharedPtr.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

namespace Niflect
{
	thread_local CMemoryStats* g_memoryStatsThreadLocal = NULL;
	
	void CMemoryStatsScope::Begin()
	{
		ASSERT(g_memoryStatsThreadLocal == NULL);
		g_memoryStatsThreadLocal = new CMemoryStats;
	}
	void CMemoryStatsScope::End()
	{
		delete g_memoryStatsThreadLocal;
		g_memoryStatsThreadLocal = NULL;
	}
	CMemoryStatsScope::CMemoryStatsScope()
	{
		Begin();
	}
	CMemoryStatsScope::~CMemoryStatsScope()
	{
		End();
	}
	
	std::mutex g_mtxPrint;
	void threadFunction()
	{
		CMemoryStatsScope memStatsScope;

		auto memTest = DebugGetMemoryStats();
		std::lock_guard<std::mutex> lg(g_mtxPrint);
		std::cout << memTest << std::endl;
	}

	void TestMemoryStatsOnThreads()
	{
		std::vector<std::thread> threads;
		for (int i = 0; i < 5; ++i)
		{
			threads.emplace_back(threadFunction);
		}

		for (auto& t : threads)
		{
			t.join();
		}
	}

	static const bool g_enabledMemoryStats_MainThreadOnly_IncludingStaticStage = false;

	static void MakeSureInitMemoryStats()
	{
		if (!g_enabledMemoryStats_MainThreadOnly_IncludingStaticStage)
			return;
		if (g_memoryStatsThreadLocal == NULL)
		{
			static CMemoryStats s_holder;
			g_memoryStatsThreadLocal = &s_holder;
		}
	}
	
	void* CDefaultMemory::Alloc(size_t size)
	{
		MakeSureInitMemoryStats();
		return CGenericMemory::Alloc(size, g_memoryStatsThreadLocal);
	}
	void* CDefaultMemory::Realloc(void* ptr, size_t size)
	{
		MakeSureInitMemoryStats();
		return CGenericMemory::Realloc(ptr, size, g_memoryStatsThreadLocal);
	}
	void CDefaultMemory::Free(void* ptr)
	{
		CGenericMemory::Free(ptr, g_memoryStatsThreadLocal);
	}

	CMemoryStats* DebugGetMemoryStats()
	{
		MakeSureInitMemoryStats();
		return g_memoryStatsThreadLocal;
	}
	
	static void TestReserved()
	{
		auto memoryTest = g_memoryStatsThreadLocal;
		auto p0 = CDefaultMemory::Alloc(4);
		ASSERT((memoryTest = g_memoryStatsThreadLocal)->m_allocatedBytesRuntimeTotal == 4);
		auto p1 = CDefaultMemory::Alloc(6);
		ASSERT((memoryTest = g_memoryStatsThreadLocal)->m_allocatedBytesRuntimeTotal == 10);
		p0 = CDefaultMemory::Realloc(p0, 6);
		ASSERT((memoryTest = g_memoryStatsThreadLocal)->m_allocatedBytesRuntimeTotal == 12);
		p1 = CDefaultMemory::Realloc(p1, 4);
		ASSERT(((memoryTest = g_memoryStatsThreadLocal)->m_allocatedBytesRuntimeTotal == 12) && ((memoryTest = g_memoryStatsThreadLocal)->m_freedBytesRuntimeTotal == 2));
		auto p2 = CDefaultMemory::Alloc(3);
		ASSERT((memoryTest = g_memoryStatsThreadLocal)->m_allocatedBytesRuntimeTotal == 15);
		CDefaultMemory::Free(p0);
		ASSERT((memoryTest = g_memoryStatsThreadLocal)->m_freedBytesRuntimeTotal == 8);
		CDefaultMemory::Free(p1);
		ASSERT((memoryTest = g_memoryStatsThreadLocal)->m_freedBytesRuntimeTotal == 12);
		CDefaultMemory::Free(p2);
		auto expectedBytesCount = 3*sizeof(size_t) + 6 + 4 + 3 + 2;//sizeof(size_t)��ʾÿ�������ڴ���Ϣͷ�ֽ���; 6, 4, 3�ֱ�Ϊp0, p1, p2�ͷ�֮ǰ���ֽ���; 2Ϊp1��6 ReallocΪ4��, �߼��ϱ��ͷŵ��ֽ���
		ASSERT(((memoryTest = g_memoryStatsThreadLocal)->m_freedBytesRuntimeTotal == 15) && ((memoryTest = g_memoryStatsThreadLocal)->m_freeCount == 3) && ((memoryTest = g_memoryStatsThreadLocal)->m_freedBytesTotal == expectedBytesCount));
	}
}