#include "NiflectGen/Collector/AccessorBindingMapping.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "NiflectGen/TaggedType/TaggedType.h"

namespace NiflectGen
{
	void CAccessorBindingMapping2::IterateForTemplate(const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, const CTaggedTypesMapping& taggedMapping, const CUntaggedTemplateTypesMapping& untaggedTemplateMapping, CResolvedCursorNode& resultIndexedParent, uint32& detailIteratingIdx) const
	{
		//auto argsCount = clang_Type_getNumTemplateArguments(fieldOrArgCXType);
		//for (int32 idx1 = 0; idx1 < argsCount; ++idx1)
		//{
		//	CXType argType = clang_Type_getTemplateArgumentAsType(fieldOrArgCXType, idx1);
		//	CBindingAccessorIndexedNode indexedChild;
		//	this->FindBindingTypeRecurs(argType, vecDetailCursor, resolvedTaggedTypeMapping, indexedChild, detailIteratingIdx);
		//	resultIndexedParent.m_vecChild.push_back(indexedChild);
		//}

		auto argsCount = clang_Type_getNumTemplateArguments(fieldOrArgCXType);
		ASSERT(!resultIndexedParent.m_isTemplateFormat);
		resultIndexedParent.m_isTemplateFormat = argsCount > 0;
		for (int32 idx1 = 0; idx1 < argsCount; ++idx1)
		{
			CXType argType = clang_Type_getTemplateArgumentAsType(fieldOrArgCXType, idx1);
			CResolvedCursorNode* indexedNext = NULL;
			if (argsCount > 1)
			{
				resultIndexedParent.m_vecChild.push_back(CResolvedCursorNode());
				indexedNext = &resultIndexedParent.m_vecChild.back();
			}
			else
			{
				//����������ʽ�����Ƿ�Ϊ�ṹģ��, ���ṹģ���ģ�����ȷʵΪ1��ʱ, ���̴���
				//����Ϊ�б�Ҫ֧��1��ģ������Ľṹģ��, Ӧʹ��һ�����ַ�ʽ, ��ͨ���� UntaggedType ���в����Ƿ�����Ӧģ����, ��ģ�����������ͬ, ����϶�Ϊ�ṹģ��
				//1ά�ṹģ����
				//AccessorBindingSetting
				//template <typename T0>
				//NIF_BS() TBindingSetting<CCompoundAccessor, TestAccessor2::TMyTransform<T0> >;
				//��Ա
				//NIF_F()
				//TestAccessor2::TMyTransform<bool> m_tm_18;
				//��ͨ�����·�ʽ����
				//auto c = clang_getTypeDeclaration(fieldOrArgCXType);
				//auto d = clang_getSpecializedCursorTemplate(c);
				//auto itFound = untaggedTemplateMapping.m_mapCursorToIndex.find(d);
				//if (itFound != untaggedTemplateMapping.m_mapCursorToIndex.end())
				//{
				//	//��һ�����ģ�������Ӧ�ĳ�Ա�����Ƿ���ģ�������ͬ, ����Ϊ��ȷָ���Ľṹģ��, ����Ϊ���������ģ��
				//	printf("");
				//}
				//����ͨ��ĳЩ�ṹ�ص��϶�, ���޷���֤��ȷ��������ģ���ṹģ��, ��Ϊ�����;���޷�ͨ���ṹ�����ƶϵ�
				//��˽��鿼�����ƿ�ܹ���
				//1. ��BindingType��1άģ��, ��һ�����϶�Ϊ����ģ��
				//2. ��BindingType�Ƕ�άģ��, ��һ�������ά��BindingType�϶�Ϊ�ṹģ��, ����Ϊ����ģ��
				//��� Main.cpp �� 12, 13 �� 14 ˵��

				resultIndexedParent.m_elem = Niflect::MakeShared<CResolvedCursorNode>();
				indexedNext = resultIndexedParent.m_elem.Get();
			}
			this->FindBindingTypeRecurs(argType, vecDetailCursor, taggedMapping, untaggedTemplateMapping, *indexedNext, detailIteratingIdx);
		}
	}
	bool CAccessorBindingMapping2::FindBindingTypesSSSSSSSSSSS(const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, uint32& detailIteratingIdx, CFoundResult& result) const
	{
		auto& foundIdx = result.m_foundIdx;
		auto& continuing = result.m_continuing;
		auto& indexedParent = result.m_indexedParent;
		Niflect::CString header;
		{
			auto itFound = m_mapCXTypeToIndex.find(fieldOrArgCXType);
			if (itFound != m_mapCXTypeToIndex.end())
				foundIdx = itFound->second;
		}
		if (foundIdx == INDEX_NONE)
		{
			//�ػ�, <Niflect::TArrayNif<bool>, ��ֱ��ͨ��field����CXType��cursor���ҵ�BindingType��cursor
			auto cursor = clang_getTypeDeclaration(fieldOrArgCXType);
			auto itFound = m_mapSpecializedCursorToIndex.find(cursor);
			if (itFound != m_mapSpecializedCursorToIndex.end())
			{
				foundIdx = itFound->second;
				header = GetCursorFilePath(cursor);
				continuing = false;
			}
		}
		if (foundIdx == INDEX_NONE)
		{
			const CXCursor* refCursor = NULL;
			while (detailIteratingIdx < vecDetailCursor.size())
			{
				auto& it = vecDetailCursor[detailIteratingIdx];
				detailIteratingIdx++;
				auto kind = clang_getCursorKind(it);
				//�����׸��ɴ�������͵�Ref
				if (kind == CXCursor_TemplateRef || kind == CXCursor_TypeRef)
				{
					refCursor = &it;
					break;
				}
			}
			if (refCursor != NULL)
			{
				auto cursor = clang_getCursorReferenced(*refCursor);
				auto itFound = m_mapCursorToIndex.find(cursor);
				if (itFound != m_mapCursorToIndex.end())
				{
					foundIdx = itFound->second;
					header = GetCursorFilePath(cursor);
					continuing = IsCursorTemplateDecl(cursor);
				}
			}
		}
		if (foundIdx != INDEX_NONE)
		{
			indexedParent.InitForAccessorBinding(foundIdx, header);
			return true;
		}
		return false;
	}
	void CAccessorBindingMapping2::FindBindingTypeRecurs(const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, const CTaggedTypesMapping& taggedMapping, const CUntaggedTemplateTypesMapping& untaggedTemplateMapping, CResolvedCursorNode& resultIndexedParent, uint32& detailIteratingIdx) const
	{
		CFoundResult result(resultIndexedParent);
		if (this->FindBindingTypesSSSSSSSSSSS(fieldOrArgCXType, vecDetailCursor, detailIteratingIdx, result))
		{
			resultIndexedParent.InitForTemplateBegin(m_vecAccessorBindingSetting[result.m_foundIdx].m_bindingTypeCursorName, result.m_foundIdx);

			if (result.m_continuing)
			{
				//auto& bindingSetting = m_vecAccessorBindingSetting[result.m_foundIdx];
				////ģ�����ػ��� BindingType, ��Ա������ Niflect::TArrayNif<Niflect::TArrayNif<bool> >
				//this->IterateForTemplate(fieldOrArgCXType, vecDetailCursor, resolvedTaggedTypeMapping, resultIndexedParent, detailIteratingIdx);
				//resultIndexedParent.InitForTemplateArguments(resultIndexedParent);

				//auto elemDeclsCount = bindingSetting.GetELementBindingTypeDeclsCount();
				//if (elemDeclsCount > 0)
				//{
				//	auto pIndexedParent = &resultIndexedParent;
				//	//��ά BindingType, ����͵� TStlMapAccessor ����, ��0άΪ TMap, ��1άΪ sd::pair
				//	for (uint32 idx0 = 0; idx0 < elemDeclsCount; ++idx0)
				//	{
				//		auto& elemSubcursor = bindingSetting.GetELementBindingTypeDecl(idx0);
				//		pIndexedParent->m_next = Niflect::MakeShared<CBindingAccessorIndexedNode>();
				//		pIndexedParent = pIndexedParent->m_next.Get();
				//		CFoundResult result2222(*pIndexedParent);
				//		uint32 aaaa = 0;
				//		if (!this->FindBindingTypesSSSSSSSSSSS(elemSubcursor.m_CXType, elemSubcursor.m_vecAaaaaaaaaa, aaaa, result2222))
				//		{
				//			ASSERT(false);//todo: ����, ÿάBindingType����Ҫָ��AccessorType
				//			break;
				//		}
				//		pIndexedParent->InitForTemplate(m_vecAccessorBindingSetting[result2222.m_foundIdx].m_bindingTypePattern, result2222.m_foundIdx, resultIndexedParent);
				//	}
				//}


				auto& bindingSetting = m_vecAccessorBindingSetting[result.m_foundIdx];

				auto elemDeclsCount = bindingSetting.GetELementBindingTypeDeclsCount();
				if (elemDeclsCount > 0)
				{
					auto pIndexedParent = &resultIndexedParent;
					//��ά BindingType, ����͵� TStlMapAccessor ����, ��0άΪ TMap, ��1άΪ sd::pair
					Niflect::TArrayNif<std::pair<CResolvedCursorNode*, uint32> > vecSSSSSSSSS;
					for (uint32 idx0 = 0; idx0 < elemDeclsCount; ++idx0)
					{
						auto& elemSubcursor = bindingSetting.GetELementBindingTypeDecl(idx0);
						pIndexedParent->m_elem = Niflect::MakeShared<CResolvedCursorNode>();
						pIndexedParent = pIndexedParent->m_elem.Get();
						CFoundResult result2222(*pIndexedParent);
						uint32 aaaa = 0;
						if (!this->FindBindingTypesSSSSSSSSSSS(elemSubcursor.m_CXType, elemSubcursor.m_vecAaaaaaaaaa, aaaa, result2222))
						{
							ASSERT(false);//todo: ����, ÿάBindingType����Ҫָ��AccessorType
							break;
						}
						//pIndexedParent->InitForTemplate(m_vecAccessorBindingSetting[result2222.m_foundIdx].m_bindingTypePattern, result2222.m_foundIdx, resultIndexedParent);
						vecSSSSSSSSS.push_back({ pIndexedParent, result2222.m_foundIdx });
					}
					this->IterateForTemplate(fieldOrArgCXType, vecDetailCursor, taggedMapping, untaggedTemplateMapping, *pIndexedParent, detailIteratingIdx);
					resultIndexedParent.InitForTemplateArguments(*pIndexedParent);

					for (auto& it : vecSSSSSSSSS)
					{
						auto& foundIdx = it.second;
						it.first->InitForTemplate(m_vecAccessorBindingSetting[foundIdx].m_bindingTypeCursorName, foundIdx, *pIndexedParent);
					}
				}
				else
				{
					//ģ�����ػ��� BindingType, ��Ա������ Niflect::TArrayNif<Niflect::TArrayNif<bool> >
					this->IterateForTemplate(fieldOrArgCXType, vecDetailCursor, taggedMapping, untaggedTemplateMapping, resultIndexedParent, detailIteratingIdx);
					resultIndexedParent.InitForTemplateArguments(resultIndexedParent);
				}
			}
			resultIndexedParent.InitForTemplateEnd();
		}
		else
		{
			auto cursor = clang_getTypeDeclaration(fieldOrArgCXType);
			taggedMapping.InitIndexedNodeForClassDecl(cursor, *this, resultIndexedParent);
		}
	}
	void CAccessorBindingMapping2::InitIndexedNodeForField(const CXCursor& fieldCursor, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, const CTaggedTypesMapping& taggedMapping, const CUntaggedTemplateTypesMapping& untaggedTemplateMapping, CResolvedCursorNode& resultIndexedParent) const
	{
		auto fieldCXType = clang_getCursorType(fieldCursor);
		uint32 detailIteratingIdx = 0;
		this->FindBindingTypeRecurs(fieldCXType, vecDetailCursor, taggedMapping, untaggedTemplateMapping, resultIndexedParent, detailIteratingIdx);
	}
	static Niflect::CString GenerateBindingTypeCursorName(const CXCursor& cursor, const CXType& type)
	{
		Niflect::CString name;
		auto kind = clang_getCursorKind(cursor);
		if ((kind != CXCursor_NoDeclFound) && (kind != CXCursor_ClassDecl))
		{
			name = GenerateNamespacesAndScopesCode(cursor);
			name += CXStringToCString(clang_getCursorSpelling(cursor));
		}
		else
		{
			name = CXStringToCString(clang_getTypeSpelling(type));
		}
		return name;
	}

	enum class EMyVisitingState
	{
		None,
		Stop,
	};
	static void GenerateTemplateInstanceCodeRecurs3(const CSubcursor& parentSubcursor, EMyVisitingState& visitingState, Niflect::CString& text)
	{
		//�ú������ʵ�� GenerateTemplateInstanceCode ��������, ��Ҫ����Ϊ
		//1. ��֧������ FullScope ������
		//2. ���� NiflectGenDefinition::CodeStyle ȷ��Ƕ��ģ���Ҽ������Ƿ�ӿո�
		//3. ����ģ�����Ͷ���, �����ػ���ǲ����ػ��� Subcursor, ���ݹ�����Ŷ�Ӧ�� CXType, ����������, �� CursorName
		Niflect::CString name;
		if (parentSubcursor.m_vecAaaaaaaaaa.size() > 0)
		{
			for (uint32 idx = 0; idx < parentSubcursor.m_vecAaaaaaaaaa.size(); ++idx)
			{
				auto& it = parentSubcursor.m_vecAaaaaaaaaa[idx];
				auto kind = clang_getCursorKind(it);
				{
					//�ֹ���д��Ӧ��Cursor(��Field, AccessorBinding)���ܱ�֤scope(namespace����scope)��������, ��˺���, ͨ��GenerateNamespacesAndScopesCode��������scope
					if (kind != CXCursor_NamespaceRef)
					{
						if (clang_isReference(kind))
						{
							auto decl = clang_getCursorReferenced(it);

							bool isArgName = clang_getCursorKind(decl) == CXCursor_TemplateTypeParameter;
							if (!isArgName)
							{
								auto strScope = GenerateNamespacesAndScopesCode(decl);
								name += strScope;
							}
							else
							{
								visitingState = EMyVisitingState::Stop;
								return;
							}
							auto spelling = CXStringToCString(clang_getCursorSpelling(decl));
							name += spelling;

							if (idx != parentSubcursor.m_vecAaaaaaaaaa.size() - 1)
								name += "::";
						}
						else
						{
							ASSERT(false);
						}
					}
				}
			}
		}
		else
		{
			auto strScope = GenerateNamespacesAndScopesCode(parentSubcursor.m_cursorDecl);
			name += strScope;
			name += GetNameFromCursorOrTypeDeclaration(parentSubcursor.m_cursorDecl, parentSubcursor.m_CXType);
		}

		text += name;
		bool canRecurs = true;
		if (parentSubcursor.m_vecAaaaaaaaaa.size() > 0)
		{
			//m_vecChildΪģ����������õ�decl, ��TypedefAliasDecl, m_vecAaaaaaaaaa.back()��Ϊģ�������Spelling����, ��ģ��ΪTypeRef, ģ��ΪTemplateRef, ���TypeRef��Ӧ�����ݹ�
			if (clang_getCursorKind(parentSubcursor.m_vecAaaaaaaaaa.back()) == CXCursor_TypeRef)
			{
				canRecurs = false;
			}
		}
		else
		{
			canRecurs = clang_getCursorKind(parentSubcursor.m_cursorDecl) != CXCursor_TypeAliasDecl;
		}
		if ((canRecurs) && (parentSubcursor.m_vecChild.size() > 0))
		{
			Niflect::CString childrenText;
			for (uint32 idx = 0; idx < parentSubcursor.m_vecChild.size(); ++idx)
			{
				Niflect::CString childText;
				GenerateTemplateInstanceCodeRecurs3(parentSubcursor.m_vecChild[idx], visitingState, childText);

				if (visitingState == EMyVisitingState::Stop)
					break;

				childrenText += childText;
				if (idx != parentSubcursor.m_vecChild.size() - 1)
					childrenText += ", ";
			}
			if (!childrenText.empty())
			{
				NiflectGenDefinition::CodeStyle::TemplateAngleBracketL(text);
				text += childrenText;
				NiflectGenDefinition::CodeStyle::TemplateAngleBracketR(text);
			}
		}
	}
	void GenerateTemplateInstanceCode3(const CSubcursor& parentSubcursor, Niflect::CString& text)
	{
		auto visitingState = EMyVisitingState::None;
		GenerateTemplateInstanceCodeRecurs3(parentSubcursor, visitingState, text);
	}
	void CAccessorBindingMapping2::InitPatterns()
	{
		for (auto& it : m_vecAccessorBindingSetting)
		{
			//auto& aSubcursor = it.GetAccessorTypeDecl();
			//it.m_accessorTypePattern = GenerateNamespacesAndScopesCode(aSubcursor.m_cursorDecl);
			//it.m_accessorTypePattern += CXStringToCString(clang_getCursorSpelling(aSubcursor.m_cursorDecl));

			//auto& bSubcursor = it.GetBindingTypeDecl();
			//it.m_bindingTypePattern = GenerateBindingTypeCursorName(bSubcursor.m_cursorDecl, bSubcursor.m_CXType);

			GenerateTemplateInstanceCode3(it.GetAccessorTypeDecl(), it.m_accessorTypeCursorName);
			GenerateTemplateInstanceCode3(it.GetBindingTypeDecl(), it.m_bindingTypeCursorName);
		}
	}
}