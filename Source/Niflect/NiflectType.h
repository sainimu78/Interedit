#pragma once
#include "Niflect/NiflectCommon.h"
#include "Niflect/NiflectRegisteredType.h"
#include "Niflect/NiflectMethod.h"
#include "Niflect/NiflectAccessor.h"
#include "Niflect/Util/TypeUtil.h"

namespace Niflect
{	
	typedef uint32 CTypeIndex;

	typedef CSharedAccessor (*InvokeCreateFieldLayoutFunc)();

	class CTypeInvokations
	{
	public:
		CTypeInvokations()
			: m_InvokeConstructorFunc(NULL)
			, m_InvokeDestructorFunc(NULL)
			, m_InvokeCreateFieldLayoutFunc(NULL)
		{
		}
		InvokeConstructorFunc m_InvokeConstructorFunc;//一些其它框架中称作 Ctor 与 Dtor
		InvokeDestructorFunc m_InvokeDestructorFunc;
		InvokeCreateFieldLayoutFunc m_InvokeCreateFieldLayoutFunc;
	};

	//Native Meta
	class CNatimeta
	{
	private:
		virtual void DebugFuncForDynamicCast() {}//仅为动态检查类型避免错误, 如已定义非调试用的virtual函数则可移除, 备注: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 
	};
	using CSharedNatimeta = TSharedPtr<CNatimeta>;

	class CNiflectType
	{
	public:
		CNiflectType()
			: m_index(INDEX_NONE)
			, m_niflectTypeSize(0)
			, m_typeHash(0)
		{
		}

	public:
		void InitTypeMeta(uint32 niflectTypeSize, size_t typeHash, const CString& name, CTypeIndex index, const CTypeInvokations& cb)
		{
			m_name = name;
			m_index = index;
			m_niflectTypeSize = niflectTypeSize;
			m_cb = cb;
			m_typeHash = typeHash;
		}
		void InitTypeMeta2(uint32 nativeTypeSize, size_t typeHash, CTypeIndex tableIdx, const CTypeInvokations& typeFuncs, const CString& nativeTypeName)
		{
			m_niflectTypeSize = nativeTypeSize;
			m_typeHash = typeHash;
			m_index = tableIdx;
			m_cb = typeFuncs;
			m_name = nativeTypeName;
		}
		
	public:
		//CField* GetRootField() const
		//{
		//	return m_fieldRoot.Get();
		//}
		//const CSharedAccessor& GetSharedAccessorRoot() const
		//{
		//	return m_accessorRoot;
		//}
		const CTypeIndex& GetTypeIndex() const//todo: 计划改名为 GetTableIndex
		{
			return m_index;
		}
		const CString& GetTypeName() const//todo: 计划改名为 GetNativeTypeName
		{
			return m_name;
		}
		const uint32& GetTypeSize() const//todo: 计划改名为 GetNativeTypeSize
		{
			return m_niflectTypeSize;//对于C++ Built in类型, 返回类型为const ref是为了方便赋值类型用auto
		}

	public:
		//void Construct(void* instanceBase) const
		//{
		//	if (m_cb.m_InvokeConstructorFunc != NULL)
		//		m_cb.m_InvokeConstructorFunc(instanceBase);
		//}
		//void Destruct(void* instanceBase) const
		//{
		//	if (m_cb.m_InvokeDestructorFunc != NULL)
		//		m_cb.m_InvokeDestructorFunc(instanceBase);
		//}
		CSharedAccessor CreateFieldLayout() const
		{
			if (m_cb.m_InvokeCreateFieldLayoutFunc != NULL)
				return m_cb.m_InvokeCreateFieldLayoutFunc();
			return NULL;
		}
		void InitFieldLayout()
		{
			ASSERT(false);//todo: 计划废弃, 缓存应包含所有继承链上的layout, 另外layout也必须根据完整layout进行相应的初始化
			//todo: 未确定方案, 用到再创建还是在Module初始化时统一遍历创建, 现用后者实验
			ASSERT(m_fieldRoot == NULL);
			m_fieldRoot = this->CreateFieldLayout();
		}
		template <typename TBase>
		TSharedPtr<TBase> MakeSharedInstance() const
		{
			//return GenericMakeShared<TBase, CMemory>(m_niflectTypeSize, m_cb.m_InvokeDestructorFunc, m_cb.m_InvokeConstructorFunc);
			return GenericPlacementMakeShared<TBase, CMemory>(m_niflectTypeSize, m_cb.m_InvokeDestructorFunc, m_cb.m_InvokeConstructorFunc);
		}
		//template <typename TBase>
		//inline TSharedPtr<TBase> MakeSharableInstance(TBase* obj) const
		//{
		//	return GenericMakeSharable<TBase, CMemory>(obj, m_cb.m_InvokeDestructorFunc);
		//}

	//public:
	//	template <typename T>
	//	inline T& GetInstanceRef(void* parentBase) const
	//	{
	//		return this->GetAccessorRoot()->GetInstanceRef<T>(parentBase);
	//	}
	public:
		//todo: 此函数可废弃, 改为静态函数获取m_typeHash
		template <typename T>
		inline T& GetInstanceRef(AddrType base) const
		{
			auto Check = [this]()
			{
				ASSERT(GetTypeHash<T>() == m_typeHash);
				return true;
			};
			ASSERT(Check());
			return *static_cast<T*>(base);
		}

	public:
		void SetNatimeta(const CSharedNatimeta& natimeta)
		{
			m_natimeta = natimeta;
		}
		CNatimeta* GetNatimeta() const
		{
			return m_natimeta.Get();
		}
		template <typename TDerived>
		TDerived* GetDerivedNatimeta() const
		{
			ASSERT(dynamic_cast<TDerived*>(m_natimeta.Get()) != NULL);
			return static_cast<TDerived*>(m_natimeta.Get());
		}

	public:
		template <typename T>
		static size_t GetTypeHash()
		{
			return typeid(T).hash_code();
		}

	private:
		virtual void DebugFuncForDynamicCast() {}//仅为动态检查类型避免错误, 如已定义非调试用的virtual函数则可移除, 备注: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 

	private:
		CString m_name;//todo: 计划改名为 m_nativeTypeName
		CTypeIndex m_index;//todo: 计划改名为 m_tableIdx;
		uint32 m_niflectTypeSize;//todo: 计划改名为 m_nativeTypeSize;
		CSharedAccessor m_fieldRoot;
		CTypeInvokations m_cb;//todo: 计划改名为 m_typeFuncs
		size_t m_typeHash;
		CSharedNatimeta m_natimeta;
	};

	using CSharedType = TSharedPtr<CNiflectType>;
	
	class CEnumMeta
	{
	public:
		TArrayNif<CString> m_vecEnumConstant;
	};

	class CEnum : public CNiflectType
	{
	public:
		void InitEnumMeta(const CEnumMeta& data)
		{
			ASSERT(m_meta.m_vecEnumConstant.size() == 0);
			m_meta = data;
		}
		const CEnumMeta& GetMeta() const
		{
			return m_meta;
		}
		const CString& GetName(uint32 idx) const
		{
			return m_meta.m_vecEnumConstant[idx];
		}
		uint32 FindIndex(const CString& name) const
		{
			for (uint32 idx = 0; idx < m_meta.m_vecEnumConstant.size(); ++idx)
			{
				if (m_meta.m_vecEnumConstant[idx] == name)
					return idx;
			}
			return INDEX_NONE;
		}

	private:
		CEnumMeta m_meta;
	};

	class CFunction : public CNiflectType
	{
	};

	class CInheritableType : public CNiflectType
	{
		typedef CNiflectType inherited;
	public:
		CInheritableType()
			: m_parent(NULL)
		{
		}
		void InitInheritableTypeMeta(CInheritableType* parent)
		{
			m_parent = parent;
		}
		CInheritableType* GetParent() const
		{
			return m_parent;
		}

	//private:
	//	static void GetStackedTypes(CNiflectType* derivedType, bool reversed, TArrayNif<CInheritableType*>& vecStackedType)
	//	{
	//		auto baseType = CInheritableType::CastChecked(derivedType);
	//		while (baseType != NULL)
	//		{
	//			if (!reversed)
	//				vecStackedType.insert(vecStackedType.begin(), baseType);
	//			else
	//				vecStackedType.push_back(baseType);
	//			baseType = baseType->GetParent();
	//		}
	//	}

	//public:
	//	inline static void GetStackedTypesConstructionOrder(CNiflectType* derivedType, TArrayNif<CInheritableType*>& vecStackedType)
	//	{
	//		GetStackedTypes(derivedType, false, vecStackedType);
	//	}
	//	inline static void GetStackedTypesDestructionOrder(CNiflectType* derivedType, TArrayNif<CInheritableType*>& vecStackedType)
	//	{
	//		GetStackedTypes(derivedType, true, vecStackedType);
	//	}

	public:
		static CInheritableType* Cast(CNiflectType* base)
		{
			ASSERT(base == NULL || dynamic_cast<CInheritableType*>(base) != NULL);
			return static_cast<CInheritableType*>(base);
		}
		static CInheritableType* CastChecked(CNiflectType* base)
		{
			return dynamic_cast<CInheritableType*>(base);
		}

	private:
		CInheritableType* m_parent;//todo: 应定义AddChild建立层级关系; 进一步地, 可能还需要另外的容器建立引用关系, 目前不确认引用关系是静态或动态建立
	};

	typedef CInheritableType CStruct;
	
	//todo: 考虑是否从CStruct继承以复用树型关系, 但m_parent是不同的, 如果复用则CClass的m_parent使用CStruct的m_parent并相应Cast, 是否可接受
	class CClass : public CInheritableType
	{
		typedef CInheritableType inherited;
	public:
		static CClass* Cast(CNiflectType* base)
		{
			ASSERT(base == NULL || dynamic_cast<CClass*>(base) != NULL);
			return static_cast<CClass*>(base);
		}

	public:
		TArrayNif<CNiflectMethod2> m_vecMethod;
	};
}