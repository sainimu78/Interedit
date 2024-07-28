#pragma once
#include "Niflect/NiflectCommon.h"
#include "Niflect/Memory/Generic/GenericInstance.h"
//#define TEST_GENERIC_SHARED_PTR
#ifdef TEST_GENERIC_SHARED_PTR
#include <stdio.h>
#endif

namespace Niflect
{
#ifdef TEST_GENERIC_SHARED_PTR
	namespace TestSharedPtr
	{
		static void TestReferences();
	}
#endif

	//简介: 最简智能指针, 本类包含一个公共智能指针数据指针空间, 公共智能指针数据包含原始指针, 引用计数与析构函数
	//主要特点说明:
	//1. 可指定Constructor与Destructor回调, 以支持反射系统实例化对象
	//2. 不要求从任何基类继承
	//3. 可通过模板参数TMemory指定内存分配管理
	//4. 严格规定与raw指针建立引用只能通过MakeShared与MkaeSharable
	//5. 引用计数减至0时, 通过MakeSharable建立引用的指针需要释放两个对象, 其一为被引用的对象, 其二为SGenericSharedPtrData引用数据, 通过MakeShared建立引用的指针只需要释放一个对象, 因为被引用对象与SGenericSharedPtrData在同一块内存上
	//6. 移植方便, 如可用于改写某些功能, 使其智能指针的对象也能接受内存管理
	//----------------------------------------------------------------------------------------------
	//与ref_ptr不同处:
	//1. 不要求从任何类继承, 即 Non-intrusive
	//2. 如32位占8字节, 公共数据占12字节; 64位占16字节, 公共数据占16字节
	//3. 无法随意直接引用原始指针
	//4. 为规范性, 只能用MakeSharable从原始指针建立引用, 可避免由于基类未定义virtual析构导致不安全释放对象
	//5. 为统一内存管理, raw指针只能通过基于TMemory的内存创建
	//6. 不计划实现线程安全的引用计数, 如Atomic add, 现认为并发的情况可用线程同步的方式避免非线程安全的引用计数
	//7. 不计划实现WeakPtr等引用, 现认为检查指针有效性是系统设计不完善的过渡方法
	//该类不可与原ref_ptr简单替换, 如果替换应调整所有直接引用原始指针处, 只能在初始化时从原始指针建立引用
	//----------------------------------------------------------------------------------------------
	//与std::shared_ptr不同处:
	//1. 在释放时, 通过MakeShared创建的SharedPtr, 不需要删除两个对象, 而std::shared_ptr需要删除引用对象与计数对象
	//2. 不支持week_ptr
	//3. 模板实例本身占用空间为2个指针, 而std::shared_ptr包含引用指针与两个引用计数
	//不考虑任意从原始指针建立引用时, 该类可与std::shared_ptr简单替换
	//----------------------------------------------------------------------------------------------
	//与UE TSharedPtr不同处:
	//1. 删除过程更简单一些, 可直接调用析构函数指针, 而UE TSharedPtr需要通过一个虚函数调用析构函数, 消耗略多一些
	//2. 其它不同处见上述关于std::shared_ptr的第2与3项

	struct SGenericSharedPtrData
	{
		uint32 m_refCount;//可改为平台相关的整数或64位整数, 现认为32位足够, 如类型改为64位整数, 则公共智能指针数据占用空间相应增加4字节
		bool m_isAllocatedByMakeShared;//为支持MakeShared, 内部确定如何删除data, 如有规范要求只能用MakeShared或MakeSharable, 则可不定义该bool以减少占用内存与略微简化释放对象的过程
		InvokeDestructorFunc m_InvokeDestructorFunc;//为通用性增加该函数指针以支持基类未定义virtual析构也能够安全释放, 如有规范要求基类必须定义virtual析构, 则可不定义该函数指针以减少占用内存
	};

	template <typename TClass, typename TMemory>
	class TGenericSharedPtr
	{
		typedef TMemory CMemory;
		typedef TClass CClass;

		template <typename TDerived, typename TSameMemory>
		friend class TGenericSharedPtr;
		template <typename TDerived, typename TMemory2, typename TConstructFunc, typename ...TArgs>
		friend TGenericSharedPtr<TDerived, TMemory2> GenericMakeShared(uint32 objSize, const InvokeDestructorFunc& DestructFunc, const TConstructFunc& ConstructFunc, TArgs&& ...args);
		template <typename TDerived, typename TMemory2>
		friend TGenericSharedPtr<TDerived, TMemory2> GenericMakeSharable(TDerived* rawPtr, const InvokeDestructorFunc& DestructFunc);

#ifdef TEST_GENERIC_SHARED_PTR
		friend void TestSharedPtr::TestReferences();
#endif

	public:
		TGenericSharedPtr()
			: m_pointer(NULL)
			, m_data(NULL)
		{
		}
		TGenericSharedPtr(const TGenericSharedPtr& rhs)
			: m_pointer(rhs.m_pointer)
			, m_data(rhs.m_data)
		{
			this->IncRef();
		}
		//支持子类赋值到父类指针
		template <typename TDerived>
		TGenericSharedPtr(const TGenericSharedPtr<TDerived, CMemory>& rhs)
			: m_pointer(rhs.m_pointer)
			, m_data(rhs.m_data)
		{
			this->IncRef();
		}
		TGenericSharedPtr(const std::nullptr_t pointer)
			: m_pointer(NULL)
			, m_data(NULL)
		{
			ASSERT(pointer == nullptr);
		}
		~TGenericSharedPtr()
		{
			this->DecRef();
		}
		template <typename TDerived>
		TGenericSharedPtr<TDerived, CMemory> Cast() const
		{
			return {static_cast<TDerived*>(m_pointer), m_data};
		}
		template <typename TDerived>
		TGenericSharedPtr<TDerived, CMemory> CastChecked() const
		{
			return { dynamic_cast<TDerived*>(m_pointer), m_data };
		}

	private:
		TGenericSharedPtr(CClass* pointer, SGenericSharedPtrData* data)
			: m_pointer(pointer)
			, m_data(data)
		{
			this->IncRef();
		}

	public:
		inline CClass* Get() const
		{
			return m_pointer;
		}

	public:
		TGenericSharedPtr& operator=(const TGenericSharedPtr& rhs)
		{
			if (m_pointer != rhs.m_pointer)
				this->AssignFrom(rhs.m_pointer, rhs.m_data);
			return *this;
		}
		CClass& operator*()
		{
			return *m_pointer;
		}
		const CClass& operator*() const
		{
			return *m_pointer;
		}
		inline CClass* operator->() const
		{
			return m_pointer;
		}
		bool operator==(const TGenericSharedPtr& rhs) const
		{
			return m_pointer == rhs.m_pointer;
		}
		bool operator!=(const TGenericSharedPtr& rhs) const
		{
			return !this->operator==(rhs);
		}
		bool operator==(const CClass* pointer) const
		{
			return m_pointer == pointer;
		}
		bool operator!=(const CClass* pointer) const
		{
			return !this->operator==(pointer);
		}
		friend bool operator==(const CClass* pointer, const TGenericSharedPtr& rhs)
		{
			return rhs.operator==(pointer);
		}
		friend bool operator!=(const CClass* pointer, const TGenericSharedPtr& rhs)
		{
			return !(rhs.operator==(pointer));
		}

	public:
		explicit operator bool() const
		{
			return m_pointer != NULL;
		}

	private:
		template <typename TDerived>
		void InitWithData(TDerived* pointer, const InvokeDestructorFunc& DestructFunc)
		{
			ASSERT(m_data == NULL);
			m_pointer = pointer;
			m_data = static_cast<SGenericSharedPtrData*>(CMemory::Alloc(sizeof(SGenericSharedPtrData)));
			StaticInitData(m_data, DestructFunc, false);
			this->IncRef();
		}
		template <typename TDerived>
		void InitWithSharedData(TDerived* pointer, const InvokeDestructorFunc& DestructFunc, SGenericSharedPtrData* data)
		{
			ASSERT(m_data == NULL);
			m_pointer = pointer;
			m_data = data;
			StaticInitData(m_data, DestructFunc, true);
			this->IncRef();
		}
		void IncRef()
		{
			if (m_data != NULL)
				m_data->m_refCount++;
		}
		void DecRef()
		{
			if (m_data != NULL)
			{
				m_data->m_refCount--;
				this->DeletePointer();
			}
		}
		inline static void StaticInitData(SGenericSharedPtrData* data, const InvokeDestructorFunc& DestructFunc, bool isAllocatedByMakeShared)
		{
			data->m_refCount = 0u;
			data->m_InvokeDestructorFunc = DestructFunc;
			data->m_isAllocatedByMakeShared = isAllocatedByMakeShared;
		}
		void DeletePointer()
		{
			if (m_data->m_refCount == 0)
			{
				if (!m_data->m_isAllocatedByMakeShared)
				{
					CGenericInstance::DestructAndFree<CMemory>(m_pointer, m_data->m_InvokeDestructorFunc);
					//UE的实现方法, 定义一种析构代理类(见DefaultDeleter), MakeShareable时使用CRT默认new创建, 在删除时使用的CRT默认delete释放
					CMemory::Free(m_data);
				}
				else
				{
					m_data->m_InvokeDestructorFunc(m_pointer);
					auto mem = reinterpret_cast<char*>(m_pointer) - sizeof(SGenericSharedPtrData);
					CMemory::Free(mem);
				}
			}
		}
		void AssignFrom(CClass* pointer, SGenericSharedPtrData* data)
		{
			this->DecRef();
			m_pointer = pointer;
			m_data = data;
			this->IncRef();
		}

	private:
		template <typename TDerived>
		inline static bool StaticCheck(TDerived& rhsRawPtr)
		{
			const TClass* thisRawPtr = rhsRawPtr;//静态检查是否为安全指针转换, 如父类转子类指针为不安全, 应先正确cast后再赋TSharedPtr
			return true;
		}

	private:
		CClass* m_pointer;
		SGenericSharedPtrData* m_data;
	};
	template <typename TDerived, typename TMemory, typename TConstructFunc, typename ...TArgs>
	static TGenericSharedPtr<TDerived, TMemory> GenericMakeShared(uint32 objSize, const InvokeDestructorFunc& DestructFunc, const TConstructFunc& ConstructFunc, TArgs&& ...args)
	{
		TGenericSharedPtr<TDerived, TMemory> shared;
		auto dataSize = sizeof(SGenericSharedPtrData);
		auto mem = TMemory::Alloc(dataSize + objSize);
		auto obj = reinterpret_cast<TDerived*>(static_cast<char*>(mem) + dataSize);
		ConstructFunc(obj, std::forward<TArgs>(args)...);
		shared.InitWithSharedData(obj, DestructFunc, static_cast<SGenericSharedPtrData*>(mem));
		return shared;
	}
	//当基类未定义virtual析构时, 为确保安全释放子类对象, 应使用MakeSharable, 在创建原始子类raw指针后立即建立引用
	//如果能确保基类定义virtual析构, 则直接使用构造函数或=都是能安全释放的
	template <typename TDerived, typename TMemory>
	inline static TGenericSharedPtr<TDerived, TMemory> GenericMakeSharable(TDerived* rawPtr, const InvokeDestructorFunc& DestructFunc)
	{
		TGenericSharedPtr<TDerived, TMemory> shared;
		ASSERT(rawPtr != NULL);
		shared.InitWithData(rawPtr, DestructFunc);
		return shared;
	}

#ifdef TEST_GENERIC_SHARED_PTR
	namespace TestSharedPtr
	{
		class CMyClass0
		{
		public:
			CMyClass0()
				: m_myFloat0(111.0f)
				, m_myFloat1(111.1f)
			{
				static int cnt = 0;
				printf("CMyClass0, constructor, %d\n", cnt++);
			}
			~CMyClass0()
			{
				static int cnt = 0;
				printf("CMyClass0, destructor, %d\n", cnt++);
			}

		public:
			float m_myFloat0;
			float m_myFloat1;
		};
		class CMyClassBase1
		{
		public:
			CMyClassBase1()
				: m_myFloatBase2(111.2f)
				, m_myFloatBase3(111.3f)
			{
				static int cnt = 0;
				printf("CMyClassBase1, constructor, %d\n", cnt++);
			}
			~CMyClassBase1()
			{
				static int cnt = 0;
				printf("CMyClassBase1, destructor, %d\n", cnt++);
			}

		public:
			float m_myFloatBase2;
			float m_myFloatBase3;
		};
		class CMyClass1 : public CMyClassBase1
		{
		public:
			CMyClass1()
				: m_myFloat4(111.4f)
				, m_myFloat5(111.5f)
			{
				static int cnt = 0;
				printf("CMyClass1, constructor, %d\n", cnt++);
			}
			CMyClass1(float arg0, bool arg1)
				: CMyClass1()
			{
			}
			~CMyClass1()
			{
				static int cnt = 0;
				printf("CMyClass1, destructor, %d\n", cnt++);
			}

		public:
			float m_myFloat4;
			CMyClass0 m_sub0;
			float m_myFloat5;
		};
		static size_t s_runtimeBytes = 0;
		
		class CTestMemory
		{
		public:
			static void* Alloc(size_t size)
			{
				auto actual = sizeof(size_t) + size;
				s_runtimeBytes += size;
				auto mem = malloc(actual);
				*static_cast<size_t*>(mem) = size;
				return static_cast<char*>(mem) + sizeof(size_t);
			}
			static void* Realloc(void* ptr, size_t size)
			{
				ASSERT(false);
				return realloc(ptr, size);
			}
			static void Free(void* ptr)
			{
				void* mem = (static_cast<char*>(ptr) - sizeof(size_t));
				const auto& size = *static_cast<size_t*>(mem);
				s_runtimeBytes -= size;
				free(mem);
			}
		};

		template <typename T>
		using TTestSharedPtr = TGenericSharedPtr<T, CTestMemory>;

		template <typename T>
		inline static TTestSharedPtr<T> TestMakeSharable(T* rawPtr)
		{
			return GenericMakeSharable<T, CTestMemory>(rawPtr, &GenericInstanceInvokeDestructor<T>);
		}
		template <typename T, typename ...TArgs>
		inline static TTestSharedPtr<T> TestMakeShared(TArgs&& ...args)
		{
			return GenericMakeShared<T, CTestMemory>(sizeof(T), &GenericInstanceInvokeDestructor<T>, &GenericInstanceInvokeConstructor<T, TArgs...>, std::forward<TArgs>(args)...);
		}

		static void TestSharedPtrConstRef(const TTestSharedPtr<CMyClass1>& obj)
		{
		}
		static void TestSharedPtrRef(TTestSharedPtr<CMyClass1>& obj)
		{
		}
		static void TestSharedPtrCopy(TTestSharedPtr<CMyClass1> obj)
		{
		}
		static void TestSharedPtrRefForError(TTestSharedPtr<CMyClass1>& obj)
		{
		}

		static void TestForCompiling()
		{
			{
				//auto a = sizeof(std::shared_ptr<float>);
				//auto b = sizeof(TGenericSharedPtrData<float>);
				//auto c = sizeof(TGenericSharedPtr<float, CMemory>);
				//auto e = sizeof(std::aligned_union<1, float>::type);
				//std::shared_ptr<float> dddddd;
				//auto g = sizeof(dddddd.use_count());
				//printf("");
			}
			//begin, TSharedPtr测试
			TTestSharedPtr<CMyClass1> a;
			a = NULL;
			a = nullptr;
			TTestSharedPtr<CMyClass1> b;
			a = b;
			TTestSharedPtr<CMyClass1> c = a;
			TTestSharedPtr<CMyClassBase1> d = a;
			TTestSharedPtr<CMyClassBase1> e;
			e = a;
			TestSharedPtrConstRef(NULL);
			TestSharedPtrConstRef(nullptr);
			TestSharedPtrConstRef(a);
			TestSharedPtrRef(a);
			TestSharedPtrCopy(NULL);
			TestSharedPtrCopy(nullptr);
			TestSharedPtrCopy(a);
			//启用以下宏后应出现编译错误, 即不允许直接通过raw指针赋值
	//#define TEST_FOR_ERROR
	#ifdef TEST_FOR_ERROR
			CMyClass1* f = NULL;
			a = f;
			TSharedPtr<CMyClass1> g = f;
			TestSharedPtrRefForError(NULL);
			TestSharedPtrRefForError(nullptr);
	#endif
			//end
		}
		void TestReferences()
		{
			auto& runtimeBytes = s_runtimeBytes;
			{
				TTestSharedPtr<CMyClass1> a = TestMakeShared<CMyClass1>();
				auto sizeWithSharedPtrUnit = sizeof(CMyClass1)+sizeof(TTestSharedPtr<CMyClass1>);
				ASSERT(a.m_data->m_refCount == 1);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit);
				auto b = TestMakeShared<CMyClass1>(1.0f, true);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit * 2);
				ASSERT(b.m_data->m_refCount == 1);
				b = a;
				ASSERT(a.m_data->m_refCount == 2);
				ASSERT(a.m_data == b.m_data);
				auto c = b;
				ASSERT(a.m_data->m_refCount == 3);
				b = NULL;
				ASSERT(a.m_data->m_refCount == 2);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit);
				CMyClass1* d = new (CTestMemory::Alloc(sizeof(CMyClass1))) CMyClass1();
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit + sizeof(CMyClass1));
				auto e = TestMakeSharable(d);
				ASSERT(e.m_data->m_refCount == 1);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit * 2);
				auto f = e;
				ASSERT(e.m_data->m_refCount == 2);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit * 2);
				f = NULL;
				ASSERT(e.m_data->m_refCount == 1);
				e = nullptr;
				ASSERT(e.m_data == NULL);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit);
			}
			ASSERT(runtimeBytes == 0);
		}
	}
#endif
}