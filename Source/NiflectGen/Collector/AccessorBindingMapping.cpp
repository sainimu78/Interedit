#include "NiflectGen/Collector/AccessorBindingMapping.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "NiflectGen/TaggedType/TaggedType.h"

namespace NiflectGen
{
	bool CAccessorBindingMapping2::IterateForTemplate(const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, const CTaggedTypesMapping& taggedMapping, const CUntaggedTemplatesMapping& untaggedTemplateMapping, CResolvedCursorNode& resultIndexedParent, uint32& detailIteratingIdx) const
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
		bool isTemplateFormat = argsCount > 0;
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
		return isTemplateFormat;
	}
	bool CAccessorBindingMapping2::InitResocursorNodeIfFound(CAccessorBindingFindingContext& ctx, CResolvedCursorNode& resocursorNode) const
	{
		uint32 foundAccessorBindingIdx = INDEX_NONE;
		uint32 foundUntaggedTemplateIndex = INDEX_NONE;
		auto& outDetailIteratingIdx = ctx.m_outDetailIteratingIdx;
		auto& continuing = ctx.m_continuing;

		Niflect::CString header;
		{
			auto itFound = m_mapCXTypeToIndex.find(ctx.m_fieldOrArgCXType);
			if (itFound != m_mapCXTypeToIndex.end())
				foundAccessorBindingIdx = itFound->second;
		}
		if (foundAccessorBindingIdx == INDEX_NONE)
		{
			//�ػ�, <Niflect::TArrayNif<bool>, ��ֱ��ͨ��field����CXType��cursor���ҵ�BindingType��cursor
			auto cursor = clang_getTypeDeclaration(ctx.m_fieldOrArgCXType);
			auto itFound = m_mapSpecializedCursorToIndex.find(cursor);
			if (itFound != m_mapSpecializedCursorToIndex.end())
			{
				foundAccessorBindingIdx = itFound->second;
				header = GetCursorFilePath(cursor);
				continuing = false;
			}
		}
		if (foundAccessorBindingIdx == INDEX_NONE)
		{
			const CXCursor* refCursor = NULL;
			while (ctx.m_outDetailIteratingIdx < ctx.m_vecDetailCursor.size())
			{
				auto& it = ctx.m_vecDetailCursor[outDetailIteratingIdx];
				outDetailIteratingIdx++;
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
					foundAccessorBindingIdx = itFound->second;
					header = GetCursorFilePath(cursor);
					bool isTemplate = IsCursorTemplateDecl(cursor);
					if (isTemplate)
					{
						auto itFound = ctx.m_untaggedTemplateMapping.m_mapCursorToIndex.find(cursor);
						ASSERT(itFound != ctx.m_untaggedTemplateMapping.m_mapCursorToIndex.end());
						foundUntaggedTemplateIndex = itFound->second;
					}
					continuing = isTemplate;
				}
			}
		}
		if (foundAccessorBindingIdx != INDEX_NONE)
		{
			resocursorNode.InitForAccessorBinding(foundAccessorBindingIdx, foundUntaggedTemplateIndex, header);
			return true;
		}
		return false;
	}
	void CAccessorBindingMapping2::FindBindingTypeRecurs(const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, const CTaggedTypesMapping& taggedMapping, const CUntaggedTemplatesMapping& untaggedTemplateMapping, CResolvedCursorNode& resultIndexedParent, uint32& detailIteratingIdx) const
	{
		CAccessorBindingFindingContext result(fieldOrArgCXType, vecDetailCursor, untaggedTemplateMapping, detailIteratingIdx);
		if (this->InitResocursorNodeIfFound(result, resultIndexedParent))
		{
			auto& bindingSetting = m_settings.m_vecAccessorBindingSetting[resultIndexedParent.m_accessorBindingIndex];
			resultIndexedParent.InitForTemplateBegin(bindingSetting.m_bindingTypeCursorName);

			if (result.m_continuing)
			{
				auto elemDeclsCount = bindingSetting.GetELementBindingTypeDeclsCount();
				if (elemDeclsCount > 0)
				{
					auto pIndexedParent = &resultIndexedParent;
					//1ά����ģ���׶�����ṹģ��� BindingType, ����͵� TStlMapAccessor ����, 1ά����ģ��Ϊ TMap, ������ṹģ�� sd::pair, ��� ContainerTemplate1D �� StructuralTemplateND �ĸ���˵��
					Niflect::TArrayNif<CResolvedCursorNode*> vecElemResocursorNode;
					for (uint32 idx0 = 0; idx0 < elemDeclsCount; ++idx0)
					{
						auto& elemSubcursor = bindingSetting.GetELementBindingTypeDecl(idx0);
						pIndexedParent->m_elem = Niflect::MakeShared<CResolvedCursorNode>();
						pIndexedParent = pIndexedParent->m_elem.Get();
						uint32 aaaa = 0;
						CAccessorBindingFindingContext result2222(elemSubcursor.m_CXType, elemSubcursor.m_vecAaaaaaaaaa, untaggedTemplateMapping, aaaa);
						if (!this->InitResocursorNodeIfFound(result2222, *pIndexedParent))
						{
							ASSERT(false);//todo: ����, ÿάBindingType����Ҫָ��AccessorType
							break;
						}
						vecElemResocursorNode.push_back(pIndexedParent);
					}
					bool isTemplateFormat = this->IterateForTemplate(fieldOrArgCXType, vecDetailCursor, taggedMapping, untaggedTemplateMapping, *pIndexedParent, detailIteratingIdx);
					resultIndexedParent.InitForTemplateArguments(*pIndexedParent, isTemplateFormat);

					for (auto& it : vecElemResocursorNode)
					{
						auto& elemSetting = m_settings.m_vecAccessorBindingSetting[it->m_accessorBindingIndex];
						it->InitForTemplate(elemSetting.m_bindingTypeCursorName, *pIndexedParent, isTemplateFormat);
					}
				}
				else
				{
					//ģ�����ػ��� BindingType, ��Ա������ Niflect::TArrayNif<Niflect::TArrayNif<bool> >
					bool isTemplateFormat = this->IterateForTemplate(fieldOrArgCXType, vecDetailCursor, taggedMapping, untaggedTemplateMapping, resultIndexedParent, detailIteratingIdx);
					resultIndexedParent.InitForTemplateArguments(resultIndexedParent, isTemplateFormat);
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
	void CAccessorBindingMapping2::InitIndexedNodeForField(const CXCursor& fieldCursor, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, const CTaggedTypesMapping& taggedMapping, const CUntaggedTemplatesMapping& untaggedTemplateMapping, CResolvedCursorNode& resultIndexedParent) const
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

	enum class EGenerateAccessorBindingCursorNameVisitingState
	{
		None,
		Stop,
	};
	static void GenerateAccessorBindingCursorNameRecurs(const CSubcursor& parentSubcursor, EGenerateAccessorBindingCursorNameVisitingState& visitingState, Niflect::CString& text)
	{
		//�ú������ʵ�� GenerateTemplateInstanceCode ��������, ��Ҫ����Ϊ
		//1. ��֧������ FullScope ������
		//2. ���� NiflectGenDefinition::CodeStyle ȷ��Ƕ��ģ���Ҽ������Ƿ�ӿո�
		//3. ����ģ�����Ͷ���, �����ػ���ǲ����ػ��� Subcursor, ���ݹ�����Ŷ�Ӧ�� CXType, ����������, �� CursorName
		//������ FindBindingTypeRecurs ��ԭ���� AccessorBinding �г��ֵ�����, ��һ������ Collect �׶�֧���ռ�������, ����� Accessor ����, ͨ�����ޱ�ǵ�
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
								visitingState = EGenerateAccessorBindingCursorNameVisitingState::Stop;
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
				GenerateAccessorBindingCursorNameRecurs(parentSubcursor.m_vecChild[idx], visitingState, childText);

				if (visitingState == EGenerateAccessorBindingCursorNameVisitingState::Stop)
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
	void GenerateAccessorBindingCursorName(const CSubcursor& parentSubcursor, Niflect::CString& text)
	{
		auto visitingState = EGenerateAccessorBindingCursorNameVisitingState::None;
		GenerateAccessorBindingCursorNameRecurs(parentSubcursor, visitingState, text);
	}
	void CAccessorBindingMapping2::InitPatterns()
	{
		for (auto& it : m_settings.m_vecAccessorBindingSetting)
		{
			//auto& aSubcursor = it.GetAccessorTypeDecl();
			//it.m_accessorTypePattern = GenerateNamespacesAndScopesCode(aSubcursor.m_cursorDecl);
			//it.m_accessorTypePattern += CXStringToCString(clang_getCursorSpelling(aSubcursor.m_cursorDecl));

			//auto& bSubcursor = it.GetBindingTypeDecl();
			//it.m_bindingTypePattern = GenerateBindingTypeCursorName(bSubcursor.m_cursorDecl, bSubcursor.m_CXType);

			GenerateAccessorBindingCursorName(it.GetAccessorTypeDecl(), it.m_accessorTypeCursorName);
			GenerateAccessorBindingCursorName(it.GetBindingTypeDecl(), it.m_bindingTypeCursorName);
		}
		if (m_settings.m_settingCompound.IsValid())
			GenerateAccessorBindingCursorName(m_settings.m_settingCompound.GetAccessorTypeDecl(), m_settings.m_settingCompound.m_accessorTypeCursorName);
		if (m_settings.m_settingEnumClass.IsValid())
			GenerateAccessorBindingCursorName(m_settings.m_settingEnumClass.GetAccessorTypeDecl(), m_settings.m_settingEnumClass.m_accessorTypeCursorName);
		if (m_settings.m_settingEnumBitMask.IsValid())
			GenerateAccessorBindingCursorName(m_settings.m_settingEnumBitMask.GetAccessorTypeDecl(), m_settings.m_settingEnumBitMask.m_accessorTypeCursorName);
	}
}