#include "NiflectGen/TaggedType/TaggedInheritableType.h"
#include "NiflectGen/Util/CursorUtil.h"
#include "NiflectGen/CodeWriter/TypeReg/InheritableTypeRegCodeWriter.h"
#include "Niflect/Util/TypeUtil.h"
#include "Niflect/Util/DebugUtil.h"
#include "NiflectGen/CodeWriter/TypeReg/InheritableTypeRegCodeWriter2.h"

namespace NiflectGen
{
	CTaggedInheritableType::CTaggedInheritableType()
		: m_baseTypeSpecifierCursor(g_invalidCursor)
		, m_baseTaggedType(NULL)
	{
	}
	void CTaggedInheritableType::InitBaseTypeSpecifierCursor(const CXCursor& cursor)
	{
		ASSERT(clang_Cursor_isNull(m_baseTypeSpecifierCursor));
		m_baseTypeSpecifierCursor = cursor;
	}
	void CTaggedInheritableType::Deprecated_ResolveDependcies(const TCursorMap<CTaggedType*>& mapCursorDeclToTaggedType)
	{
		ASSERT(m_baseTaggedType == NULL);
		auto baseTypeCursorDecl = clang_getTypeDeclaration(clang_getCursorType(m_baseTypeSpecifierCursor));
		auto itFound = mapCursorDeclToTaggedType.find(baseTypeCursorDecl);
		if (itFound != mapCursorDeclToTaggedType.end())
			m_baseTaggedType = itFound->second;
	}
	static void DebugPrintIndexedNodeRecurs(const CResolvedCursorNode& indexedParent, const CResolvedCursorNode& childrenOwner, const CAccessorBindingMapping2& mapping, uint32 lv)
	{
		auto strLv = NiflectUtil::DebugIndentToString(lv);
		auto& cxType = mapping.m_vecAccessorBindingSetting[indexedParent.m_accessorBindingIndex].GetAccessorTypeDecl().m_CXType;
		auto a = CXStringToCString(clang_getTypeSpelling(cxType));
		printf("%s%s\n", strLv.c_str(), a.c_str());

		lv++;
		for (auto& it : childrenOwner.m_vecChild)
			DebugPrintIndexedNodeRecurs(it, it, mapping, lv);

		if (auto elem = indexedParent.m_elem.Get())
		{
			lv--;
			printf("---------------\n");
			DebugPrintIndexedNodeRecurs(*elem, indexedParent, mapping, lv);
		}
	}
	void CTaggedInheritableType::ResolveDependcies(const CResolvingDependenciesContext& context, SResolvingDependenciesData& data)
	{
		inherited::ResolveDependcies(context, data);

		//����
		ASSERT(m_baseTaggedType == NULL);
		auto baseTypeCursorDecl = clang_getTypeDeclaration(clang_getCursorType(m_baseTypeSpecifierCursor));
		auto& taggedMapping = context.m_mappings.m_tagged;
		auto itFound = taggedMapping.m_mapCursorToIndex.find(baseTypeCursorDecl);
		if (itFound != taggedMapping.m_mapCursorToIndex.end())
			m_baseTaggedType = taggedMapping.m_vecType[itFound->second];

		//��Ա����
		ASSERT(m_vecMember.size() == 0);
		for (auto& it : m_vecChild)
		{
			//Ƕ������ҲΪtaggedType���ӽڵ�
			if (auto member = CTaggedInheritableTypeMember::CastChecked(it.Get()))
				m_vecMember.push_back(member);
		}

		m_vecMemberIndexedRoot.resize(m_vecMember.size());
		for (uint32 idx0 = 0; idx0 < m_vecMember.size(); ++idx0)
		{
			auto& it0 = m_vecMember[idx0];
			auto& indexedRoot = m_vecMemberIndexedRoot[idx0];
			auto& fieldCursor = it0->GetCursor();
			context.m_mappings.m_accessorBinding.InitIndexedNodeForField(fieldCursor, it0->m_vecDetailCursor, taggedMapping, context.m_mappings.m_untaggedTemplate, indexedRoot);
			if (!indexedRoot.IsValid())
			{
				//���ƻ�֧�� Field ������Ϊ BindingType �ı���, ��Ϊ�����������ñ���, ������ AccessorBinding �Ķ�Ӧ��ϵ��ֱ��, ����Ҫʹ�ñ���, Ӧ�� AccessorBinding ��ָ��, �� Field ��ʹ��ʱ, Ӧֱ��ʹ�ñ���
				//����Ϊȷʵ��Ҫʵ��֧�ִ��÷�, �ɿ����ڲ��ҹ�����, ������ aliasChain ���Ҷ�Ӧ�� BindingType
				GenLogError(context.m_log, NiflectUtil::FormatString("The accessor of the field %s::%s is not specified", m_resocursorName.c_str(), CXStringToCString(clang_getCursorSpelling(fieldCursor)).c_str()));
				break;
			}
			//DebugPrintIndexedNodeRecurs(indexedRoot, indexedRoot, context.m_bindingAccessorMapping, 0);
			ResolveSignature(indexedRoot, context, data.m_signatureMapping);
		}
	}
	CSharedTypeRegCodeWriter CTaggedInheritableType::Deprecated_CreateCodeWriter(const STypeRegClassWritingSetting& setting) const
	{
		CXCursor baseTypeCursorDecl = g_invalidCursor;
		if (m_baseTaggedType != NULL)
			baseTypeCursorDecl = m_baseTaggedType->GetCursor();
		Niflect::TArrayNif<CTaggedInheritableTypeMember*> vecMember;
		for (auto& it : m_vecChild)
		{
			//Ƕ������ҲΪtaggedType���ӽڵ�
			if (auto member = CTaggedInheritableTypeMember::CastChecked(it.Get()))
				vecMember.push_back(member);
		}
		return Niflect::MakeShared<CInheritableTypeRegCodeWriter_ObjectAccessor>(this->GetCursor(), setting, baseTypeCursorDecl, vecMember);
	}
	CSharedTypeRegCodeWriter CTaggedInheritableType::CreateCodeWriter(const STypeRegClassWritingSetting& setting) const
	{
		CXCursor baseTypeCursorDecl = g_invalidCursor;
		if (m_baseTaggedType != NULL)
			baseTypeCursorDecl = m_baseTaggedType->GetCursor();
		return Niflect::MakeShared<CInheritableTypeRegCodeWriter_ObjectAccessor>(this->GetCursor(), setting, baseTypeCursorDecl, m_vecMember);
	}
	CSharedTypeRegCodeWriter2 CTaggedInheritableType::CreateCodeWriter2() const
	{
		//CXCursor baseTypeCursorDecl = g_invalidCursor;
		//if (m_baseTaggedType != NULL)
		//	baseTypeCursorDecl = m_baseTaggedType->GetCursor();
		//return Niflect::MakeShared<CInheritableTypeRegCodeWriter_ObjectAccessor>(this->GetCursor(), setting, baseTypeCursorDecl, m_vecMember);

		return Niflect::MakeShared<CInheritableTypeRegCodeWriter2>(m_vecMemberIndexedRoot, m_baseTaggedType);
	}
	void CTaggedInheritableType::DebugDerivedPrint(FILE* fp) const
	{
		Niflect::CString baseClassStr;
		const auto& baseTypeCursor = m_baseTypeSpecifierCursor;
		if (clang_Cursor_isNull(baseTypeCursor))
			baseClassStr = "No base class";
		else
			baseClassStr = NiflectUtil::FormatString("Base class name: %s", CXStringToCString(clang_getCursorSpelling(baseTypeCursor)).c_str());
		printf(", %s", baseClassStr.c_str());
	}
}