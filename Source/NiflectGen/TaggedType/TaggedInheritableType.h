#pragma once
#include "NiflectGen/TaggedType/TaggedType.h"

namespace NiflectGen
{
	class CTaggedInheritableType : public CTaggedType
	{
	public:
		CTaggedInheritableType();

	protected:
		void InitBaseTypeSpecifierCursor(const CXCursor& cursor);

	public:
		virtual void Deprecated_ResolveDependcies(const TCursorMap<CTaggedType*>& mapCursorDeclToTaggedType) override;
		virtual void ResolveDependcies(const CResolvingDependenciesContext& context, SResolvingDependenciesData& data) override;
		virtual CSharedTypeRegCodeWriter Deprecated_CreateCodeWriter(const STypeRegClassWritingSetting& setting) const override;
		virtual CSharedTypeRegCodeWriter CreateCodeWriter(const STypeRegClassWritingSetting& setting) const override;

	protected:
		virtual void DebugDerivedPrint(FILE* fp) const override;

	private:
		CXCursor m_baseTypeSpecifierCursor;
		CTaggedType* m_baseTaggedType;
		Niflect::TArrayNif<CTaggedInheritableTypeMember*> m_vecMember;
		CBindingAccessorIndexedNode m_classDeclIndexedRoot;
		Niflect::TArrayNif<CBindingAccessorIndexedNode> m_vecMemberIndexedRoot;
	};
}