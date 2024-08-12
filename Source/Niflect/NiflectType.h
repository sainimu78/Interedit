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
		InvokeConstructorFunc m_InvokeConstructorFunc;//һЩ��������г��� Ctor �� Dtor
		InvokeDestructorFunc m_InvokeDestructorFunc;
		InvokeCreateFieldLayoutFunc m_InvokeCreateFieldLayoutFunc;
	};

	class CTypeCustomData
	{
	};
	using CSharedTypeCustomData = TSharedPtr<CTypeCustomData>;

	//todo: ������Ҫ��factory����factory����, ��factory�а�module, ��˼�����������֯, �Ա��ȡ������Ϣ
	//todo: �����Ƿ���Ҫ�����const��CNiflectType, �����ⲿʹ��
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
		//template <typename T>
		//void InitStaticType()
		//{
		//	TInternalRegisteredType<T>::s_type = this;
		//}
		void InitTypeMeta(uint32 niflectTypeSize, size_t typeHash, const CString& name, CTypeIndex index, const CTypeInvokations& cb)
		{
			m_name = name;
			m_index = index;
			m_niflectTypeSize = niflectTypeSize;
			m_cb = cb;
			m_typeHash = typeHash;
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
		const CTypeIndex& GetTypeIndex() const
		{
			return m_index;
		}
		const CString& GetTypeName() const
		{
			return m_name;
		}
		const uint32& GetTypeSize() const
		{
			return m_niflectTypeSize;//����C++ Built in����, ��������Ϊconst ref��Ϊ�˷��㸳ֵ������auto
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
			//todo: δȷ������, �õ��ٴ���������Module��ʼ��ʱͳһ��������, ���ú���ʵ��
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
		//todo: �˺����ɷ���, ��Ϊ��̬������ȡm_typeHash
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
		void SetCustomData(const CSharedTypeCustomData& data)
		{
			m_customData = data;
		}
		CTypeCustomData* GetCustomData() const
		{
			return m_customData.Get();
		}
		template <typename TCustomData>
		TCustomData* GetDerivedCustomData() const
		{
			return static_cast<TCustomData*>(m_customData.Get());
		}

	public:
		template <typename T>
		static size_t GetTypeHash()
		{
			return typeid(T).hash_code();
		}

	private:
		virtual void DebugFuncForDynamicCast() {}//��Ϊ��̬������ͱ������, ���Ѷ���ǵ����õ�virtual��������Ƴ�, ��ע: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 

	private:
		CString m_name;
		CTypeIndex m_index;
		uint32 m_niflectTypeSize;
		CSharedAccessor m_fieldRoot;
		//todo: ʵ�ֳ�Ա��������(CFunction), �����Ƿ��п������б�Ҫʵ��֧�����⹹�캯��
		CTypeInvokations m_cb;
		size_t m_typeHash;
		CSharedTypeCustomData m_customData;
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
		CInheritableType* m_parent;//todo: Ӧ����AddChild�����㼶��ϵ; ��һ����, ���ܻ���Ҫ����������������ù�ϵ, Ŀǰ��ȷ�����ù�ϵ�Ǿ�̬��̬����
	};

	typedef CInheritableType CStruct;
	
	//todo: �����Ƿ��CStruct�̳��Ը������͹�ϵ, ��m_parent�ǲ�ͬ��, ���������CClass��m_parentʹ��CStruct��m_parent����ӦCast, �Ƿ�ɽ���
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