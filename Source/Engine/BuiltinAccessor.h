#pragma once
#include "Niflect/NiflectAccessor.h"
#include "Niflect/Util/TypeUtil.h"

namespace Engine
{
	using namespace RwTree;

	template <typename TValue>
	class TBasicTypeAccessor : public Niflect::CAccessor
	{
	public:
		virtual bool SaveToRwNode2222(const AddrType offsetBase, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TValue*>(offsetBase);
			ASSERT(!rw->IsValue());
			auto rwValue = rw->ToValue();
			SetRwValueAs<TValue>(rwValue, instance);
			return true;
		}
		virtual bool LoadFromRwNode2222(AddrType offsetBase, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TValue*>(offsetBase);
			ASSERT(rw->IsValue());
			auto rwValue = rw->GetValue();
			instance = GetRwValueAs<TValue>(rwValue);
			return true;
		}
	};

	using CBoolAccessor = TBasicTypeAccessor<bool>;
	using CUint8Accessor = TBasicTypeAccessor<uint8>;
	using CFloatAccessor = TBasicTypeAccessor<float>;
	//typedef TBasicTypeAccessor<float> CFloatAccessor;
	using CStringAccessor = TBasicTypeAccessor<Niflect::CString>;
	using CStdStringAccessor = TBasicTypeAccessor<std::string>;

	class CArrayAccessor : public Niflect::CAccessor
	{
	public:
		//ר�ŷ�װ Cast ��Ϊ��ʾ������Ϊ���֧�ŵ�����, ��Ҫ�� Cast
		static CArrayAccessor* CastChecked(Niflect::CAccessor* base)
		{
			return dynamic_cast<CArrayAccessor*>(base);
		}
		static const CArrayAccessor* CastChecked(const Niflect::CAccessor* base)
		{
			return dynamic_cast<const CArrayAccessor*>(base);
		}
	};

	template <typename TStlArray>
	static const AddrType GetElementBaseToRead(const TStlArray& array, uint32 idx, bool& stlBoolItemHandler);

	template <typename TStlArray>
	static AddrType GetElementBaseToWriteBegin(TStlArray& array, uint32 idx, bool& stlBoolItemHandler);

	template <typename TStlArray>
	static void GetElementBaseToWriteEnd(TStlArray& array, uint32 idx, bool& stlBoolItemHandler);

	template <typename TStlArray>
	inline const AddrType GetElementBaseToRead(const TStlArray& array, uint32 idx, bool& stlBoolItemHandler)
	{
		return &array[idx];
	}
	//���޷�ʵ�� g++, std=c++14 �µ�, ͬʱ���� std::vector �� Niflect::TArrayNif ���ػ�, ��˷���, �������ο�
	template <>
	inline const AddrType GetElementBaseToRead(const Niflect::TArrayNif<bool>& array, uint32 idx, bool& stlBoolItemHandler)
	{
		stlBoolItemHandler = array[idx];
		return &stlBoolItemHandler;
	}
	template <typename TStlArray>
	inline AddrType GetElementBaseToWriteBegin(TStlArray& array, uint32 idx, bool& stlBoolItemHandler)
	{
		return &array[idx];
	}
	template <>
	inline AddrType GetElementBaseToWriteBegin(Niflect::TArrayNif<bool>& array, uint32 idx, bool& stlBoolItemHandler)
	{
		return &stlBoolItemHandler;
	}
	template <typename TStlArray>
	inline void GetElementBaseToWriteEnd(TStlArray& array, uint32 idx, bool& stlBoolItemHandler)
	{
	}
	template <>
	inline void GetElementBaseToWriteEnd(Niflect::TArrayNif<bool>& array, uint32 idx, bool& stlBoolItemHandler)
	{
		array[idx] = stlBoolItemHandler;
	}

	template <typename TStlArray>
	class TStlArrayAccessor_HandledBitsBasedBoolArray : public CArrayAccessor
	{
	public:
		virtual bool SaveToRwNode2222(const AddrType offsetBase, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TStlArray*>(offsetBase);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			auto elemAccessor = this->GetElementAccessor();
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = CreateRwNode();
				//auto elemBase = &instance[idx];//��std::vector<bool>�޷�֧��, ��˶��ⶨ���ػ�ģ�� GetElementBaseToX, Ҳ�ɸ���std::vector<uint8>, ������Accessor
				bool stlBoolItemHandler;
				auto elemBase = GetElementBaseToRead<TStlArray>(instance, idx, stlBoolItemHandler);
				if (elemAccessor->SaveToRwNode(elemBase, rwItem.Get()))
					rwArray->AddItem(rwItem);
			}
			return true;
		}
		virtual bool LoadFromRwNode2222(AddrType offsetBase, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TStlArray*>(offsetBase);
			ASSERT(rw->IsArray());
			auto rwArray = rw->GetArray();
			auto elemAccessor = this->GetElementAccessor();
			instance.resize(rwArray->GetItemsCount());
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = rwArray->GetItem(idx);
				//auto elemBase = &instance[idx];
				bool stlBoolItemHandler;
				auto elemBase = GetElementBaseToWriteBegin<TStlArray>(instance, idx, stlBoolItemHandler);
				if (elemAccessor->LoadFromRwNode(elemBase, rwItem))
					GetElementBaseToWriteEnd<TStlArray>(instance, idx, stlBoolItemHandler);
			}
			return true;
		}
	};

	template <typename TStlArray>
	class TStlArrayAccessor : public CArrayAccessor
	{
	public:
		virtual bool SaveToRwNode2222(const AddrType offsetBase, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TStlArray*>(offsetBase);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			auto elemAccessor = this->GetElementAccessor();
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = CreateRwNode();
				auto elemBase = &instance[idx];//��std::vector<bool>�޷�֧��, ��˶��ⶨ���ػ�ģ�� GetElementBaseToX, Ҳ�ɸ���std::vector<uint8>, ������Accessor
				if (elemAccessor->SaveToRwNode(elemBase, rwItem.Get()))
					rwArray->AddItem(rwItem);
			}
			return true;
		}
		virtual bool LoadFromRwNode2222(AddrType offsetBase, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TStlArray*>(offsetBase);
			ASSERT(rw->IsArray());
			auto rwArray = rw->GetArray();
			auto elemAccessor = this->GetElementAccessor();
			instance.resize(rwArray->GetItemsCount());
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = rwArray->GetItem(idx);
				auto elemBase = &instance[idx];
				elemAccessor->LoadFromRwNode(elemBase, rwItem);
			}
			return true;
		}
	};

	class CBitsArrayAccessor : public Niflect::CAccessor
	{
	public:
		//ר�ŷ�װ Cast ��Ϊ��ʾ������Ϊ���֧�ŵ�����, ��Ҫ�� Cast
		static CBitsArrayAccessor* CastChecked(Niflect::CAccessor* base)
		{
			return dynamic_cast<CBitsArrayAccessor*>(base);
		}
		static const CBitsArrayAccessor* CastChecked(const Niflect::CAccessor* base)
		{
			return dynamic_cast<const CBitsArrayAccessor*>(base);
		}
	};

	//todo: Bits Array Ӧ��Ϊ�������л�, �粻��Ҫ�����������, �ò�ͬ��С����������, ��ʡ�ռ�
	template <typename TStlArray>
	class TStlBitsArrayAccessor : public CBitsArrayAccessor
	{
	public:
		virtual bool SaveToRwNode2222(const AddrType offsetBase, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TStlArray*>(offsetBase);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			for (auto idx = 0; idx < instance.size(); ++idx)
				rwArray->AddItemBool(instance[idx]);
			return true;
		}
		virtual bool LoadFromRwNode2222(AddrType offsetBase, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TStlArray*>(offsetBase);
			ASSERT(rw->IsArray());
			auto rwArray = rw->GetArray();
			instance.resize(GetRwItemsCount(rwArray));
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = rwArray->GetItem(idx)->ToValue();
				instance[idx] = rwItem->GetBool();
			}
			return true;
		}
	};

	class CMapAccessor : public Niflect::CAccessor
	{
	public:
		//ר�ŷ�װ Cast ��Ϊ��ʾ������Ϊ���֧�ŵ�����, ��Ҫ�� Cast
		static CMapAccessor* CastChecked(Niflect::CAccessor* base)
		{
			return dynamic_cast<CMapAccessor*>(base);
		}
		static const CMapAccessor* CastChecked(const Niflect::CAccessor* base)
		{
			return dynamic_cast<const CMapAccessor*>(base);
		}
	};

	template <typename TStlMap>
	class TStlMapAccessor : public CMapAccessor
	{
		using TElem = typename TStlMap::allocator_type::value_type;
	public:
		virtual bool SaveToRwNode2222(const AddrType offsetBase, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TStlMap*>(offsetBase);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			auto elemAccessor = this->GetElementAccessor();
			for (auto& it : instance)
			{
				auto rwItem = CreateRwNode();
				auto elemBase = &it;
				if (elemAccessor->SaveToRwNode(elemBase, rwItem.Get()))
					rwArray->AddItem(rwItem);
			}
			return true;
		}
		virtual bool LoadFromRwNode2222(AddrType offsetBase, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TStlMap*>(offsetBase);
			ASSERT(rw->IsArray());
			auto rwArray = rw->GetArray();
			auto elemAccessor = this->GetElementAccessor();
			auto cnt = rwArray->GetItemsCount();
			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				auto rwItem = rwArray->GetItem(idx);
				TElem item;
				auto elemBase = &item;
				if (elemAccessor->LoadFromRwNode(elemBase, rwItem))
					instance.insert(item);
			}
			return true;
		}
	};
}