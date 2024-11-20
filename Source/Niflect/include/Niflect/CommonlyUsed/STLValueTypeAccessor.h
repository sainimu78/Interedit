#pragma once
#include "Niflect/NiflectAccessor.h"

namespace Niflect
{
	class CSTLStringAccessor : public CAccessor
	{
	public:
		virtual bool SaveInstanceImpl(const AddrType base, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const std::string*>(base);
			ASSERT(!rw->IsValue());
			auto rwValue = rw->ToValue();
			Niflect::CString str = instance.c_str();
			SetRwValueAs(rwValue, str);
			return true;
		}
		virtual bool LoadInstanceImpl(AddrType base, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<std::string*>(base);
			ASSERT(rw->IsValue());
			auto rwValue = rw->GetValue();
			instance = GetRwValueAs<Niflect::CString>(rwValue).c_str();
			return true;
		}
	};
}