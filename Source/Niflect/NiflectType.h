#pragma once
#include "Niflect/NiflectCommon.h"
#include "Niflect/NiflectRegisteredType.h"
#include "Niflect/NiflectMethod.h"
#include "Niflect/NiflectAccessor.h"
#include "Niflect/Util/TypeUtil.h"

namespace Niflect
{	
	typedef uint32 CTypeIndex;

	template <typename TType>
	CSharedAccessor CreateTypeAccessor();

	typedef CSharedAccessor (*CreateTypeAccessorFunc)();
	using CreateFieldLayoutOfTypeFuncOld = CreateTypeAccessorFunc;

	class CTypeInvokations
	{
	public:
		CTypeInvokations()
			: m_InvokeConstructorFunc(NULL)
			, m_InvokeDestructorFunc(NULL)
			, m_CreateTypeAccessorFunc(NULL)
		{
		}
		InvokeConstructorFunc m_InvokeConstructorFunc;//һЩ��������г��� Ctor �� Dtor
		InvokeDestructorFunc m_InvokeDestructorFunc;
		CreateTypeAccessorFunc m_CreateTypeAccessorFunc;
	};

	//Native Meta
	class CNata
	{
	private:
		virtual void DebugFuncForDynamicCast() {}//��Ϊ��̬������ͱ������, ���Ѷ���ǵ����õ�virtual��������Ƴ�, ��ע: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 
	};
	using CSharedNata = TSharedPtr<CNata>;

	class CNiflectType;
	using CStaticNiflectTypeAddr = CNiflectType*;

	class CNiflectType
	{
	public:
		CNiflectType()
			: m_index(INDEX_NONE)
			, m_niflectTypeSize(0)
			, m_typeHash(0)
			, m_staticTypePtrAddr(NULL)
		{
		}
		~CNiflectType()
		{
			if (m_staticTypePtrAddr != NULL)
				*m_staticTypePtrAddr = NULL;
		}

	public:
		void InitTypeMeta(uint32 niflectTypeSize, size_t typeHash, const CString& name, CTypeIndex index, const CTypeInvokations& cb)
		{
			ASSERT(false);
			m_name = name;
			m_index = index;
			m_niflectTypeSize = niflectTypeSize;
			m_cb = cb;
			m_typeHash = typeHash;
		}
		void InitTypeMeta2(uint32 nativeTypeSize, size_t typeHash, CTypeIndex tableIdx, const CTypeInvokations& typeFuncs, const CString& id, CStaticNiflectTypeAddr* staticTypePtrAddr)
		{
			m_niflectTypeSize = nativeTypeSize;
			m_typeHash = typeHash;
			m_index = tableIdx;
			m_cb = typeFuncs;
			m_name = id;
			m_staticTypePtrAddr = staticTypePtrAddr;
			*m_staticTypePtrAddr = this;
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
		const CTypeIndex& GetTypeIndex() const//todo: �ƻ�����Ϊ GetTableIndex
		{
			return m_index;
		}
		const CString& GetTypeName() const//todo: �ƻ�����Ϊ GetNativeTypeName
		{
			return m_name;
		}
		const uint32& GetTypeSize() const//todo: �ƻ�����Ϊ GetNativeTypeSize
		{
			return m_niflectTypeSize;//����C++ Built in����, ��������Ϊconst ref��Ϊ�˷��㸳ֵ������auto
		}

	public:
		bool SaveInstanceToRwNode(const AddrType base, CRwNode* rw) const
		{
			for (auto& it : m_layout.m_vecAccessor)
			{
				if (!it->SaveToRwNode(base, rw))
					return false;
			}
			return true;
		}
		bool LoadInstanceFromRwNode(AddrType base, const CRwNode* rw) const
		{
			for (auto& it : m_layout.m_vecAccessor)
			{
				if (!it->LoadFromRwNode(base, rw))
					return false;
			}
			return true;
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
		CSharedAccessor CreateFieldLayout() const//todo:�ƻ�ɾ��
		{
			ASSERT(false);
			if (m_cb.m_CreateTypeAccessorFunc != NULL)
				return m_cb.m_CreateTypeAccessorFunc();
			return NULL;
		}
		CSharedAccessor CreateAccessor() const
		{
			ASSERT(false);
			if (m_cb.m_CreateTypeAccessorFunc != NULL)
				return m_cb.m_CreateTypeAccessorFunc();
			return NULL;
		}
		void InitFieldLayout()
		{
			ASSERT(false);//todo: �ƻ�����, ����Ӧ�������м̳����ϵ�layout, ����layoutҲ�����������layout������Ӧ�ĳ�ʼ��
			//todo: δȷ������, �õ��ٴ���������Module��ʼ��ʱͳһ��������, ���ú���ʵ��
			//ASSERT(m_fieldRoot == NULL);
			//m_fieldRoot = this->CreateFieldLayout();
		}
		void InitTypeLayout()
		{
			ASSERT(m_layout.m_vecAccessor.size() == 0);
			this->CreateTypeLayout(m_layout);
		}
		void InitAddFieldToAccessor(CAccessor* owner, const Niflect::CString& name, const AddrOffsetType& offset) const
		{
			CField field;
			field.Init(name);
			this->CreateTypeLayout(field.m_layout);
			for (auto& it1 : field.m_layout.m_vecAccessor)
				it1->InitOffset(offset);
			owner->InitAddField(field);
		}
		void InitSetElementLayoutToAccessor(CAccessor* owner) const
		{
			CTypeLayout layout;
			this->CreateTypeLayout(layout);
			owner->InitElementLayout(layout);
		}

	protected:
		virtual void CreateTypeLayout(CTypeLayout& layout) const
		{
			if (m_cb.m_CreateTypeAccessorFunc != NULL)
				layout.m_vecAccessor.push_back(m_cb.m_CreateTypeAccessorFunc());
		}

	public:
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
		void InitNata(const CSharedNata& nata)
		{
			m_nata = nata;
		}
		CNata* GetNata() const
		{
			return m_nata.Get();
		}
		template <typename TDerived>
		TDerived* GetDerivedNata() const
		{
			ASSERT(dynamic_cast<TDerived*>(m_nata.Get()) != NULL);
			return static_cast<TDerived*>(m_nata.Get());
		}

	public:
		template <typename T>
		static size_t GetTypeHash()
		{
			return typeid(T).hash_code();
		}

	//private:
	//	virtual void DebugFuncForDynamicCast() {}//��Ϊ��̬������ͱ������, ���Ѷ���ǵ����õ�virtual��������Ƴ�, ��ע: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 

	protected:
		CTypeInvokations m_cb;//todo: �ƻ�����Ϊ m_typeFuncs

	private:
		//todo: �ƻ�����Ϊ m_id, ͨ��Ϊ natiev type name
		//NiflectGen �����ɵ� id Ϊ�� namespace �� type name, ������� NIFLECT_TYPE_REGISTER ��ע��� id �� namespace, ������Ҫ��������װ.
		//����Ҫ��ȷһ��, id ���������ڱ����ظ������л�ʹ��, ʵ���Ƿ�� namespace ͨ���޹ؽ�Ҫ, һ�������ͨ��������������ּ���
		CString m_name;

		CTypeIndex m_index;//todo: �ƻ�����Ϊ m_tableIdx;
		uint32 m_niflectTypeSize;//todo: �ƻ�����Ϊ m_nativeTypeSize;
		size_t m_typeHash;
		CSharedNata m_nata;
		CStaticNiflectTypeAddr* m_staticTypePtrAddr;
		CTypeLayout m_layout;
	};
	using CSharedNiflectType = TSharedPtr<CNiflectType>;
	
	class CEnumMeta
	{
	public:
		TArrayNif<CString> m_vecEnumConstant;
	};

	class CEnum : public CNiflectType
	{
		typedef CNiflectType inherited;
	public:
		void InitEnumMeta(const CEnumMeta& data)
		{
			m_enumMeta = data;
		}
		const CEnumMeta& GetEnumMeta() const
		{
			return m_enumMeta;
		}
		const CString& GetEnumConstNameByIndex(uint32 idx) const
		{
			return m_enumMeta.m_vecEnumConstant[idx];
		}
		uint32 FindIndexByEnumConstName(const CString& name) const
		{
			for (uint32 idx = 0; idx < m_enumMeta.m_vecEnumConstant.size(); ++idx)
			{
				if (m_enumMeta.m_vecEnumConstant[idx] == name)
					return idx;
			}
			return INDEX_NONE;
		}
		template <typename TEnumType>
		const CString& GetEnumConstName(const TEnumType& e) const
		{
			auto idx = static_cast<uint32>(e);
			return this->GetEnumConstNameByIndex(idx);
		}
		template <typename TEnumType>
		TEnumType FindEnumConstByName(const CString& name) const
		{
			auto idx = this->FindIndexByEnumConstName(name);
			return static_cast<TEnumType>(idx);
		}

	public:
		static CEnum* Cast(inherited* base)
		{
			ASSERT(dynamic_cast<CEnum*>(base) != NULL);
			return static_cast<CEnum*>(base);
		}

	private:
		CEnumMeta m_enumMeta;
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

	protected:
		virtual void CreateTypeLayout(CTypeLayout& layout) const
		{
			inherited::CreateTypeLayout(layout);
			if (m_parent != NULL)
			{
				ASSERT(layout.m_vecAccessor.size() == 1);
				auto par = m_parent;
				while (par != NULL)
				{
					ASSERT(par->m_cb.m_CreateTypeAccessorFunc != NULL);
					layout.m_vecAccessor.insert(layout.m_vecAccessor.begin(), par->m_cb.m_CreateTypeAccessorFunc());
					par = par->m_parent;
				}
			}
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
			ASSERT(dynamic_cast<CInheritableType*>(base) != NULL);
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
			ASSERT(dynamic_cast<CClass*>(base) != NULL);
			return static_cast<CClass*>(base);
		}

	public:
		TArrayNif<CNiflectMethod2> m_vecMethod;
	};
}