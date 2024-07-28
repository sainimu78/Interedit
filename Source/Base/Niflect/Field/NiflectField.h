#pragma once
#include "Niflect/Property/NiflectProperty.h"
#include "Niflect/Base/NiflectAddr.h"
#include "Niflect/Util/TypeUtil.h"

namespace Niflect
{
	using namespace TreeReadWrite2;
	
	template <typename T>
	static CNiflectType* StaticGetType();

	class CNiflectType;
	
	class CField;

	using CSharedField = TSharedPtr<CField>;
	
	class IAccessorSaveToRwNode
	{
	public:
		virtual bool Do(const CField* field, const AddrType& base, CRwNode& rw) const = 0;
	};
	using CSharedAccessorSaveToRwNode = TSharedPtr<IAccessorSaveToRwNode>;

	class IAccessorLoadFromRwNode
	{
	public:
		virtual bool Do(const CField* field, AddrType& base, const CRwNode& rw) const = 0;
	};
	using CSharedAccessorLoadFromRwNode = TSharedPtr<IAccessorLoadFromRwNode>;
	
	class CCreatingPropertyTreeContext
	{
	public:
		CCreatingPropertyTreeContext()
			: m_requiredPathBasedSerializer(false)
		{
		}
		void SetRequiredPathBasedSerializer(bool requiredPathBasedSerializer)
		{
			m_requiredPathBasedSerializer = requiredPathBasedSerializer;
		}
		bool m_requiredPathBasedSerializer;
	};

	class IAccessorCreatePropertyTree
	{
	public:
		virtual CSharedProperty Do(CField* field, const AddrType& base, const CCreatingPropertyTreeContext& context) const = 0;
	};
	using CSharedAccessorCreatePropertyTree = TSharedPtr<IAccessorCreatePropertyTree>;
	
	class IAccessorGetter
	{
	public:
		virtual bool SaveToRwNode(const CField* field, const AddrType& base, CRwNode& rw) const = 0;
		virtual CSharedProperty CreatePropertyTree(const CField* field, const AddrType& base, const CCreatingPropertyTreeContext& context) const = 0;
	};
	using CSharedAccessorGetter = TSharedPtr<IAccessorGetter>;
	
	class IAccessorSetter
	{
	public:
		virtual bool LoadFromRwNode(const CField* field, AddrType& base, const CRwNode& rw) const = 0;
	};
	using CSharedAccessorSetter = TSharedPtr<IAccessorSetter>;

	template<typename TOwner, typename Ret, typename ...Args>
	class TAccessorMethodHasConst
	{
		using function_type = Ret (TOwner::*)(Args...) const;

	public:
		TAccessorMethodHasConst(const function_type &f) : m_func(f) {}

		Ret Invoke(void* owner, Args... args) const
		{
			return (static_cast<TOwner*>(owner)->*m_func)(std::forward<Args>(args)...);
		}

	private:
		function_type m_func;
	};
	template<typename TOwner, typename Ret, typename ...Args>
	class TAccessorMethodNonConst
	{
		using function_type = Ret (TOwner::*)(Args...);

	public:
		TAccessorMethodNonConst(const function_type &f) : m_func(f) {}

		Ret Invoke(void* owner, Args... args) const
		{
			return (static_cast<TOwner*>(owner)->*m_func)(std::forward<Args>(args)...);
		}

	private:
		function_type m_func;
	};
	
	class CAccessorAddrSaveToRwNode : public IAccessorSaveToRwNode
	{
	public:
		CAccessorAddrSaveToRwNode(const CAddrOffset& addrOffset);
		
	public:
		virtual bool Do(const CField* field, const AddrType& base, CRwNode& rw) const override;

	private:
		const CAddrOffset& m_addrOffset;
	};
	
	class CAccessorAddrLoadFromRwNode : public IAccessorLoadFromRwNode
	{
	public:
		CAccessorAddrLoadFromRwNode(const CAddrOffset& addrOffset);
		
	public:
		virtual bool Do(const CField* field, AddrType& base, const CRwNode& rw) const override;

	private:
		const CAddrOffset& m_addrOffset;
	};
	
	class CAccessorAddrCreatePropertyTree : public IAccessorCreatePropertyTree
	{
	public:
		CAccessorAddrCreatePropertyTree(const CAddrOffset& addrOffset);
		
	public:
		virtual CSharedProperty Do(CField* field, const AddrType& base, const CCreatingPropertyTreeContext& context) const override;

	private:
		const CAddrOffset& m_addrOffset;
	};
	
	class CAccessorAddrGetter : public IAccessorGetter
	{
	public:
		CAccessorAddrGetter(const CAddrOffset& addrOffset);
		
	public:
		virtual bool SaveToRwNode(const CField* field, const AddrType& base, CRwNode& rw) const override;
		virtual CSharedProperty CreatePropertyTree(const CField* field, const AddrType& base, const CCreatingPropertyTreeContext& context) const override;

	private:
		const CAddrOffset& m_addrOffset;
	};
	
	class CAccessorAddrSetter : public IAccessorSetter
	{
	public:
		CAccessorAddrSetter(const CAddrOffset& addrOffset);
		
	public:
		virtual bool LoadFromRwNode(const CField* field, AddrType& base, const CRwNode& rw) const override;

	private:
		const CAddrOffset& m_addrOffset;
	};

	class CField
	{
	public:
		CField()
			: m_type(NULL)
			, m_owner(NULL)
			//, m_saveToRwNode(MakeShared<CAccessorAddrSaveToRwNode>(m_addrOffset))//Ϊ����δʹ�õ�Method based accessor, �ڹ���ʱ����addr based accessor
			//, m_loadFromRwNode(MakeShared<CAccessorAddrLoadFromRwNode>(m_addrOffset))
			//, m_createPropertyTree(MakeShared<CAccessorAddrCreatePropertyTree>(m_addrOffset))
		{
		}

	public:
		void InitType(CNiflectType* type)
		{
			m_type = type;
		}
		void InitMemberMeta(const CString& name, const AddrOffsetType& offset, CField* owner/*����*/)
		{
			m_name = name;
			m_addrOffset.SetOffset(offset);
			//m_owner = owner;
		}
		void InitMemberMeta_ReservedForMethodBasedAccessor(const CString& name, CField* owner/*����*/)
		{
			//�ú���������ش���Ϊ֧��Method based accessor���òο�, ��ʵ�ֵ�����accessor֧��ƫ�����Ա�������õķ��䷽ʽ
			//
			//�ֲ��ƻ�֧������accessor��ԭ������
			//ʵ������:
			//1. ����Map, Ҫ���Ա�����ܹ���ĳ����ʽ�ṩiterator, ��������ʵ�ֱ���map, ��ʵ�ʱ�����, �ṩiterator�����������
			//2. ����Compound, Array��ȷʵ�Ѿ�ʵ�ֳ�Ա�������ַƫ�Ƽ��ݵķ��䷽��, ��ʵ�ַǳ�����, ���ڽ϶��ظ��߼��Ĵ���
			//3. �����޷�Ҫ���û������巵��copy��getter, �󲿷����̶���Ҫ�ڵݹ麯���н���, ��͵��¶���϶��ظ��߼��Ĵ���
			//��Ա����accessorȱ��:
			//1. ��Flax�ĳ�Ա����accessor����ͬʱ����getter��setter, ����������ش���; ����API_FIELD��getter setter��API_PROPERTYͬʱʹ�ý������ظ������л�����
			//	1. ʵ�ʱ�����, ��������Ϊ��Ҫ���л���Ҫ���û�����setter��getter
			//2. UE��֧�ֳ�Ա����accessor
			//3. ʵ�ʵķ������ʵ�������̲���Ҫ��setter�ķ�ʽ�޸�load�������, Ӧ����ʵ�����������޸�, �����ĺô����ڲ�������������߼�
			//4. ���ڱ���ʱ��ֵֻ��ͨ��getter��ȡ, ���getter���ص���copy����ζ�����ӽڵ�ı���Ҳ�����ڴ�copy�Ļ����Ͻ���, ���Ϊ����һ��ĩ�˽ڵ��Ҫͨ���Ӹ��ڵ㿪ʼ��һ������·���ݹ��������ָ����getter���ջ�����������������base

			m_name = name;
			//m_owner = owner;
		}
		CNiflectType* GetType() const
		{
			return m_type;
		}
		const CString& GetName() const
		{
			return m_name;
		}
		//template <typename T>
		//inline T& GetInstanceRef(const AddrType& base) const
		//{
		//	return *static_cast<T*>(this->GetBaseAddr(base));
		//}

	public:
		void AddChild(const CSharedField& field)
		{
			this->InsertChild(field, this->GetChildrenCount());
		}
		void InsertChild(const CSharedField& field, uint32 idx)
		{
			m_vecChild.insert(m_vecChild.begin() + idx, field);
		}
		uint32 GetChildrenCount() const
		{
			return static_cast<uint32>(m_vecChild.size());
		}
		CField* GetChild(uint32 idx) const
		{
			return m_vecChild[idx].Get();
		}

	public:
		//void InitAccessors()
		//{
		//	this->InitAccessorAddrSaveToRwNode();
		//	this->InitAccessorAddrLoadSaveToRwNode();
		//	this->InitAccessorAddrCreatePropertyTree();
		//}
		//void InitAccessorsAddrBased(const AddrOffsetType& offset)
		//{
		//	this->InitOffset(offset);
		//	this->InitAccessors();
		//}
		//void InitAccessorAddrSaveToRwNode()
		//{
		//	this->InitAccessorSaveToRwNode(MakeShared<CAccessorAddrSaveToRwNode>(m_addrOffset));
		//}
		//void InitAccessorAddrLoadSaveToRwNode()
		//{
		//	this->InitAccessorLoadFromRwNode(MakeShared<CAccessorAddrLoadFromRwNode>(m_addrOffset));
		//}
		//void InitAccessorAddrCreatePropertyTree()
		//{
		//	this->InitAccessorCreatePropertyTree(MakeShared<CAccessorAddrCreatePropertyTree>(m_addrOffset));
		//}
		void InitOffset(const AddrOffsetType& offset)
		{
			m_addrOffset.SetOffset(offset);
		}
		//void InitAccessorSaveToRwNode(const CSharedAccessorSaveToRwNode& accessor)
		//{
		//	ASSERT(m_saveToRwNode == NULL);
		//	m_saveToRwNode = accessor;
		//}
		//void InitAccessorLoadFromRwNode(const CSharedAccessorLoadFromRwNode& accessor)
		//{
		//	ASSERT(m_loadFromRwNode == NULL);
		//	m_loadFromRwNode = accessor;
		//}
		//void InitAccessorCreatePropertyTree(const CSharedAccessorCreatePropertyTree& accessor)
		//{
		//	ASSERT(m_createPropertyTree == NULL);
		//	m_createPropertyTree = accessor;
		//}
		
		void InitAccessors()
		{
			this->InitAccessorAddrGetter();
			this->InitAccessorAddrSetter();
		}
		void InitAccessorsAddrBased(const AddrOffsetType& offset)
		{
			this->InitOffset(offset);
			this->InitAccessors();
		}
		void InitAccessorAddrGetter()
		{
			this->InitAccessorGetter(MakeShared<CAccessorAddrGetter>(m_addrOffset));
		}
		void InitAccessorAddrSetter()
		{
			this->InitAccessorSetter(MakeShared<CAccessorAddrSetter>(m_addrOffset));
		}
		void InitAccessorGetter(const CSharedAccessorGetter& getter)
		{
			ASSERT(m_getter == NULL);
			m_getter = getter;
		}
		void InitAccessorSetter(const CSharedAccessorSetter& setter)
		{
			ASSERT(m_setter == NULL);
			m_setter = setter;
		}

	public:
		bool SaveToRwNode(const AddrType& base, CRwNode& rw) const
		{
			return m_getter->SaveToRwNode(this, base, rw);
			//return m_saveToRwNode->Do(this, base, rw);
		}
		bool LoadFromRwNode(AddrType& base, const CRwNode& rw) const
		{
			return m_setter->LoadFromRwNode(this, base, rw);
			//return m_loadFromRwNode->Do(this, base, rw);
		}
		CSharedProperty CreatePropertyTree(const AddrType& base, const CCreatingPropertyTreeContext& context = CCreatingPropertyTreeContext())
		{
			return m_getter->CreatePropertyTree(this, base, context);
			//return m_createPropertyTree->Do(this, base, context);
		}

	public:
		virtual bool FieldSaveToRwNode(const AddrType& base, CRwNode& rw) const = 0;
		virtual bool FieldLoadFromRwNode(AddrType& base, const CRwNode& rw) const = 0;
		virtual bool FieldInitProperty(const AddrType& base, CProperty* prop, const CCreatingPropertyTreeContext& context) const = 0;

	private:
		CString m_name;
		CNiflectType* m_type;
		TArrayNif<CSharedField> m_vecChild;
		CField* m_owner;
		CAddrOffset m_addrOffset;
		CSharedAccessorSaveToRwNode m_saveToRwNode;
		CSharedAccessorLoadFromRwNode m_loadFromRwNode;
		CSharedAccessorCreatePropertyTree m_createPropertyTree;
		CSharedAccessorGetter m_getter;
		CSharedAccessorSetter m_setter;
	};
	
	inline CAccessorAddrSaveToRwNode::CAccessorAddrSaveToRwNode(const CAddrOffset& addrOffset)
		: m_addrOffset(addrOffset)
	{
	}

	inline bool CAccessorAddrSaveToRwNode::Do(const CField* field, const AddrType& base, CRwNode& rw) const
	{
		auto obj = m_addrOffset.GetAddr(base);
		return field->FieldSaveToRwNode(obj, rw);
	}
	
	inline CAccessorAddrLoadFromRwNode::CAccessorAddrLoadFromRwNode(const CAddrOffset& addrOffset)
		: m_addrOffset(addrOffset)
	{
	}
	inline bool CAccessorAddrLoadFromRwNode::Do(const CField* field, AddrType& base, const CRwNode& rw) const
	{
		auto obj = m_addrOffset.GetAddr(base);
		return field->FieldLoadFromRwNode(obj, rw);
	}
	
	template <typename TCB>
	class TAccessorMemberFunctionSaveToRwNode : public IAccessorSaveToRwNode
	{
	public:
		TAccessorMemberFunctionSaveToRwNode(const TCB& Func)
			: m_Func(Func)
		{
		}
		
	public:
		virtual bool Do(const CField* field, const AddrType& base, CRwNode& rw) const override
		{
			const auto& instance = m_Func.Invoke(base);
			AddrType addr = const_cast<AddrType>(static_cast<const void*>(&instance));
			return field->FieldSaveToRwNode(addr, rw);
		}

	private:
		TCB m_Func;
	};

	template <typename TValue, typename TCB>
	class TAccessorMemberFunctionLoadFromRwNode : public IAccessorLoadFromRwNode
	{
	public:
		TAccessorMemberFunctionLoadFromRwNode(const TCB& Func)
			: m_Func(Func)
		{
		}
		
	public:
		virtual bool Do(const CField* field, AddrType& base, const CRwNode& rw) const override
		{
			TValue instance;
			AddrType addr = static_cast<AddrType>(&instance);
			auto ret = field->FieldLoadFromRwNode(addr, rw);
			m_Func.Invoke(base, instance);
			return ret;
		}

	private:
		TCB m_Func;
	};
	
	template <typename TField, typename TType>
	struct SFieldCreator
	{
		static TSharedPtr<TField> Create()
		{
			return MakeShared<TField>();
		}
	};

	template <typename TField, typename TType>
	static TSharedPtr<TField> CreateField()
	{
		auto shared = SFieldCreator<TField, TType>::Create();
		auto field = shared.Get();
		field->InitType(StaticGetType<TType>());
		return shared;
	}
}