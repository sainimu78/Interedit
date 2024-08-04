#pragma once
#include "Niflect/NiflectTable.h"
//#include "Niflect/Field/ArrayField.h"
//#include "Niflect/Field/MapField.h"

namespace Niflect
{
	class CNiflectRegistration;

	using CSharedRegistration = TSharedPtr<CNiflectRegistration>;

	class CNiflectRegistration
	{
	public:
		CNiflectRegistration()
			: m_miscTableIndex(INDEX_NONE)
		{
		}

	public:
		void InitTables()
		{
			this->DoInitTables();
			this->AddMiscTable();
		}
		void RegisterTypes()
		{
			this->DoRegisterTypes();
		}
		void InitTypes() const
		{
			this->DoInitTypes();
		}
		void InitMethods() const
		{
			this->DoInitMethods();
		}
		void InitTypesAccessorTree()
		{
			//#3, ����AccessorTree, ��Ҫ�������л�
			for (uint32 idx0 = 0; idx0 < this->GetTablesCount(); ++idx0)
			{
				auto& table = this->GetMutableTable(idx0);
				for (auto& it : table.m_vecType)
					it->InitFieldLayout();
			}
		}

	protected:
		virtual void DoInitTables() = 0;
		virtual void DoRegisterTypes() = 0;
		virtual void DoInitTypes() const = 0;
		virtual void DoInitMethods() const = 0;

	public:
		void ClearTables()//todo: ʵ���ϻ���Ҫͨ�����ù�ϵ��ȫɾ��
		{
			m_vecTable.clear();
		}
		uint32 GetTablesCount() const
		{
			return static_cast<uint32>(m_vecTable.size());
		}
		CNiflectTable& GetMutableTable(uint32 idx)
		{
			return m_vecTable[idx];
		}
		CNiflectTable& GetMutableMiscTable()
		{
			return m_vecTable[m_miscTableIndex];
		}

	protected:
		CNiflectTable* AddNewTable()
		{
			m_vecTable.push_back(CNiflectTable());
			return &m_vecTable.back();
		}

	private:
		void AddMiscTable()
		{
			ASSERT(m_miscTableIndex == INDEX_NONE);
			m_miscTableIndex = this->GetTablesCount();
			this->AddNewTable();
		}

	public:
		static CNiflectRegistration* StaticGet()
		{
			return s_reg.Get();
		}
		template <typename TDerivedReg>
		static void StaticCreate()
		{
			s_reg = MakeShared<TDerivedReg>();
		}
		static void StaticDestroy()
		{
			s_reg = NULL;
		}

	private:
		TArrayNif<CNiflectTable> m_vecTable;
		uint32 m_miscTableIndex;

	private:
		static CSharedRegistration s_reg;
	};

	template <typename TField, typename TType>
	static CSharedField __InternalUseForFunctionPointer()
	{
		return CreateField<TField, TType>();
	}

	template <typename TField, typename TType>
	static CNiflectType* StaticGetType_Misc(const CString& typeName)
	{
		if (!TInternalRegisteredType<TType>::IsValid())
		{
			auto reg = CNiflectRegistration::StaticGet();
			auto& table = reg->GetMutableMiscTable();
			ASSERT(!typeName.empty());
			table.RegisterType<CNiflectType, TType>(typeName, &__InternalUseForFunctionPointer<TField, TType>, typeid(TType).hash_code());
		}
		return StaticGetType<TType>();
	}
}

NIFLECT_C_API Niflect::CNiflectRegistration* GetNiflectModuleRegistration();