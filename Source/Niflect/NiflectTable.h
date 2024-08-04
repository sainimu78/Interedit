#pragma once
#include "Niflect/NiflectType.h"

namespace Niflect
{
	//ΪModule������CType������, ����Ҫ���̳�
	class CNiflectTable
	{
		friend class CNiflectModule;
	public:
		void Init(const CString& name)
		{
			m_name = name;
		}
		template <typename TInfo, typename TType>
		void RegisterType(const CString& typeName, const InvokeCreateFieldLayoutFunc& Func, size_t typeHash)
		{
			CTypeInvokations typeFuncs;
			typeFuncs.m_InvokeConstructorFunc = &GenericInstanceInvokeConstructor<TType>;
			typeFuncs.m_InvokeDestructorFunc = &GenericInstanceInvokeDestructor<TType>;
			typeFuncs.m_InvokeCreateFieldLayoutFunc = Func;

			auto shared = MakeShared<TInfo>();
			auto type = shared.Get();
			auto idx = this->AddType(shared);
			ASSERT(!TInternalRegisteredType<TType>::IsValid());
			TInternalRegisteredType<TType>::s_type = type;
			//type->InitStaticType<TType>();
			type->InitTypeMeta(sizeof(TType), typeHash, typeName, idx, typeFuncs);
			ASSERT(TInternalRegisteredType<TType>::IsValid());
		}
		uint32 GetTypesCount() const
		{
			return static_cast<uint32>(m_vecType.size());//todo: ������α���cast
		}
		CNiflectType* GetTypeByIndex(uint32 idx) const
		{
			return m_vecType[idx].Get();
		}
		
	private:
		uint32 AddType(const CSharedType& type)//����VS����, ��ʱ��My��׺
		{
			uint32 idx = this->GetTypesCount();
			m_vecType.push_back(type);
			return idx;
		}

	public:
		TArrayNif<CSharedType> m_vecType;
		CString m_name;
	};

	using CSharedTable2 = TSharedPtr<CNiflectTable>;
	
	template <typename TInfo, typename TType>
	class TStaticTableTypeReg
	{
	public:
		TStaticTableTypeReg(CNiflectTable* table, const CString& typeName)
		{
			table->RegisterType<TInfo, TType>(typeName, NULL, CNiflectType::GenerateTypeHash<TType>());
		}
		template <typename TAnyCustomData>
		TStaticTableTypeReg(CNiflectTable* table, const CString& typeName, const TAnyCustomData& customData)
			: TStaticTableTypeReg(table, typeName)
		{
			auto type = StaticGetType<TType>();
			type->SetCustomData(MakeShared<TAnyCustomData>(customData));
		}
	};

#define NIFLECT_MEMBER_FUNCTION_GET_TYPE_VIRTUAL()\
	public:\
		virtual Niflect::CNiflectType* GetType() const { return NULL; }

#define NIFLECT_MEMBER_FUNCTION_GET_TYPE_OVERRIDE(typeName)\
	public:\
		virtual Niflect::CNiflectType* GetType() const override\
		{\
			return Niflect::StaticGetType<typeName>();\
		}
	
//begin, ����������, ����Ҫ���Ǿ�̬��������ͻ, ��Ϊ���Ͳ�����ͬnamespace�г���ͬ������
//#define NIFLECT_STATIC_TYPE_REG_VAR_NAME_COMBINE_INNER(A, B) s_typeReg_##A##_##B
//#define NIFLECT_STATIC_TYPE_REG_VAR_NAME_COMBINE(A, B) NIFLECT_STATIC_TYPE_REG_VAR_NAME_COMBINE_INNER(A, B)
//#define NIFLECT_STATIC_TYPE_REG_VAR_NAME(typeName) NIFLECT_STATIC_TYPE_REG_VAR_NAME_COMBINE(typeName, __LINE__)
//end

#define NIFLECT_REGISTER(niflectType, typeName, invokeGetTable, customDataObject)\
	static Niflect::TStaticTableTypeReg<niflectType, typeName> s_typeReg_##typeName(invokeGetTable, #typeName, customDataObject);
}