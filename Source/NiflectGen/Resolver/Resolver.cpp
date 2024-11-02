#include "NiflectGen/Resolver/Resolver.h"
#include "Niflect/Util/DebugUtil.h"
#include <algorithm>
#include "Niflect/Util/TypeUtil.h"
#include "NiflectGen/CodeWriter/ModuleReg/ModuleRegCodeWriter.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "NiflectGen/CodeWriter/CppWriter.h"

namespace NiflectGen
{
	using namespace Niflect;

	CResolver::CResolver(const CCollectionData& collectionData, const CModuleRegInfoValidated& moduleRegInfo)
		: m_collectionData(collectionData)
		//, m_vecHeaderSearchPath(vecHeaderSearchPath)
		, m_foundCursorsCount(0)
		, m_moduleRegInfo(moduleRegInfo)
	{
	}
	//void CResolver::Resolve(CTaggedNode2* taggedRoot, CResolvingContext& context, CResolvedData& data)
	//{
	//	this->ResolveRecurs(taggedRoot, context, data);

	//	//for (auto& it : m_collectionData.m_vecTemplateSubcursorRoot)
	//	//{
	//	//	auto accessorCursor = clang_getTypeDeclaration(it.m_vecChild[0].m_type);
	//	//	auto accessorTypeName = CXStringToCString(clang_getCursorSpelling(accessorCursor));
	//	//	CXType bindingType = it.m_vecChild[1].m_vecChild[0].m_type;
	//	//	auto bindingTypeCursor = clang_getTypeDeclaration(bindingType);
	//	//	auto bindingTypeName = CXStringToCString(clang_getCursorSpelling(bindingTypeCursor));
	//	//	//auto bindingTypeName = CXStringToCString(clang_getTypeSpelling(bindingType));
	//	//	printf("%s <-> %s\n", accessorTypeName.c_str(), bindingTypeName.c_str());
	//	//}

	//	for (auto& it0 : m_collectionData.m_vecBindingSetting)
	//	{
	//		if (!it0.IsValidBindingSetting())
	//		{
	//			GenLogError(context.m_log, "Unable to obtain type bindings from the TBindingSetting template instance");
	//			break;
	//		}
	//		auto accessorCursor = clang_getTypeDeclaration(it0.GetAccessorType());
	//		auto itFoundTaggedType = data.m_mapDeclarationToTaggedType.find(accessorCursor);
	//		if (itFoundTaggedType == data.m_mapDeclarationToTaggedType.end())
	//		{
	//			if (false)
	//			{
	//				//�ֲ�֧��ָ�������ʹ���ģ������Scope, ���¶���, m_mapDeclarationToTaggedType�е�keyΪģ�嶨��, ��accessorCursor��Ϊģ��ʵ��, �����accessorCursor����ʧ��

	//				//template <typename T>
	//				//class CFieldTestMyTemplate : public Niflect::CFieldAccessor
	//				//{
	//				//public:
	//				//	NIF_T()
	//				//		class CSubMy2 : public Niflect::CFieldAccessor
	//				//	{
	//				//	};
	//				//};
	//				// 
	//				//NIF_BS() TBindingSetting2<MySub::CFieldTestMyTemplate<float>::CSubMy2, TTypeTuple<char> >;

	//				for (auto& it000 : data.m_mapDeclarationToTaggedType)
	//				{
	//					auto a = CXStringToCString(clang_getCursorSpelling(it000.first));
	//					if (a == "CSubMy2")
	//					{
	//						auto b = clang_equalCursors(it000.first, accessorCursor);//���Ϊ, false
	//						auto c = CXStringToCString(clang_getCursorDisplayName(accessorCursor));
	//						auto d = CXStringToCString(clang_getCursorDisplayName(it000.first));
	//						auto e = clang_getCursorType(accessorCursor);
	//						auto f = clang_getCursorType(it000.first);
	//						auto g = CXStringToCString(clang_getTypeSpelling(e));//���Ϊ, MyTestClassScope::MySub::CFieldTestMyTemplate<float>::CSubMy2
	//						auto h = CXStringToCString(clang_getTypeSpelling(f));//���Ϊ, MyTestClassScope::MySub::CFieldTestMyTemplate::CSubMy2
	//						printf("");
	//					}
	//					printf("%s\n", a.c_str());
	//				}
	//			}

	//			//todo: ��Ҫ��Accessor��������, ����ʧ����3�����
	//			//1. Accessorδ�ӱ��
	//			//2. û�ж���Accessor, �����뱾���޷�����
	//			//3. ʹ��ģ��ʵ��Scope
	//			//�ƻ���ΪҪ��Accessor�����3������Accessor�̳�, �������ɸ��ݼ������ɴ��뼴��, �����Ƿ�Ϊ�����������������

	//			GenLogError(context.m_log, "The bound accessor type is not tagged");//, or not defined
	//			break;
	//		}
	//		auto& taggedAccessorType = itFoundTaggedType->second;
	//		for (auto& it1 : it0.GetBindingTypesData())
	//		{
	//			const auto& bindingType = it1.m_type;
	//			auto bindingTypeCursor = clang_getTypeDeclaration(bindingType);
	//			bool ok = false;
	//			if (clang_getCursorKind(bindingTypeCursor) == CXCursor_NoDeclFound)
	//			{
	//				if (bindingType.kind == CXType_Pointer)
	//				{
	//					GenLogError(context.m_log, "Pointer is not supported");//todo: ֧������ָ��������ʵ����;, Ӧ֧���ض����͵�ָ��, ��Ҫ��ȡ����Ϣ�缸άָ����ԭʼ����, �ƻ��ӵ���m_mapUserTypePointer1D��, ����ָ�����Ϊר�ŵ�����, ����ר�ŵ�ָ����ҪRuntime�ڴ����
	//					break;//todo: ����2��for, ��Ҫ��������߼�����д����򲻱���sibling
	//				}
	//				else
	//				{
	//					ok = data.m_accessorTypeTable.m_mapBuiltinTypeKind.insert({ bindingType.kind, taggedAccessorType }).second;
	//					if (ok)
	//					{
	//						for (uint32 idx2 = 0; idx2 < data.m_vecResolvedTypes.size(); ++idx2)
	//						{
	//							bool found = false;
	//							auto& vec = data.m_vecResolvedTypes[idx2].m_vecResolvedTaggedType;
	//							for (uint32 idx3 = 0; idx3 < vec.size(); ++idx3)
	//							{
	//								auto& resolvedTaggedType = vec[idx3];
	//								ASSERT(resolvedTaggedType.m_taggedType != NULL);
	//								if (resolvedTaggedType.m_taggedType == taggedAccessorType)
	//								{
	//									ASSERT(false);//m_isBuiltinType���Ƴ�
	//									//ASSERT(!resolvedTaggedType.m_isBuiltinType);
	//									//resolvedTaggedType.m_isBuiltinType = true;//todo: �����ޱ�Ҫ, ���ڽ���ʱ�Ѱ����û���BindingSetting.h��include DFS, ���builtin���͵�accessorһ������������builtin���͵�����֮ǰ
	//									found = true;
	//									break;
	//								}
	//							}
	//							if (found)
	//								break;
	//						}
	//					}
	//				}
	//			}
	//			else
	//			{
	//				ok = data.m_accessorTypeTable.m_mapUserType.insert({ bindingTypeCursor, taggedAccessorType }).second;
	//			}
	//			if (!ok)
	//			{
	//				GenLogError(context.m_log, "Binding types are conflicting, an identical binding type is expected for an accessor");
	//				break;//todo: ����2��for, ��Ҫ��������߼�����д����򲻱���sibling
	//			}
	//		}
	//	}
	//}
	//void CResolver::ResolveRecurs(CTaggedNode2* taggedParent, CResolvingContext& context, CResolvedData& data)
	//{
	//	if (auto taggedType = CTaggedType::CastChecked(taggedParent))
	//	{
	//		auto& cursor = taggedType->GetCursor();
	//		{
	//			auto loc = clang_getCursorLocation(cursor);
	//			CXFile file;
	//			clang_getSpellingLocation(loc, &file, NULL, NULL, NULL);
	//			auto filePathAbs = CXStringToCString(clang_getFileName(file));
	//			std::replace(filePathAbs.begin(), filePathAbs.end(), '\\', '/');
	//			auto ret = m_mapFilePathToResolvedTypeIndex.insert({ filePathAbs, static_cast<uint32>(data.m_vecResolvedTypes.size()) });
	//			if (ret.second)
	//			{
	//				CResolvedTypes item;
	//				item.m_filePathAbs = filePathAbs;
	//				bool foundPrefix = false;
	//				for (auto& it : m_vecHeaderSearchPath)
	//				{
	//					ASSERT(!it.empty());
	//					auto pos = filePathAbs.find(it);
	//					if (pos != std::string::npos)
	//					{
	//						auto prefixLength = pos + it.length();
	//						item.m_filePathRelativeToHeaderSearchPath = filePathAbs.substr(prefixLength + 1, filePathAbs.length() - prefixLength);
	//						foundPrefix = true;
	//						break;
	//					}
	//				}
	//				if (!foundPrefix)
	//				{
	//					GenLogError(context.m_log, "Unable to resolve the header relative path");
	//					return;//todo: ���ڵݹ�, ��Ҫ��������߼�����д����򲻱���sibling
	//				}
	//				data.m_vecResolvedTypes.emplace_back(item);
	//			}
	//			auto& item = data.m_vecResolvedTypes[ret.first->second];
	//			item.m_vecResolvedTaggedType.push_back(taggedType);
	//		}
	//		ASSERT(clang_isDeclaration(clang_getCursorKind(cursor)));
	//		auto ret = data.m_mapDeclarationToTaggedType.insert({ cursor, taggedType });
	//		if (!ret.second)
	//		{
	//			GenLogError(context.m_log, "Tagged type collection goes wrong");
	//			return;//todo: ���ڵݹ�, ��Ҫ��������߼�����д����򲻱���sibling
	//		}
	//	}

	//	for (auto& it0 : taggedParent->DebugGetChildren())
	//	{
	//		this->ResolveRecurs(it0.Get(), context, data);
	//	}
	//}
	void CResolver::Deprecated_Resolve2(CTaggedNode2* taggedRoot, CResolvingContext& context, CResolvedData& data)
	{
		auto& accessorBindingMapping = data.deprecated_m_mapping.m_accessorBindingMapping;
		for (auto& it0 : m_collectionData.deprecated_m_vecBindingSetting)
		{
			if (!it0.Deprecated_IsValidBindingSetting())
			{
				GenLogError(context.m_log, "Unable to obtain type bindings from the TBindingSetting template instance");
				break;
			}
			
			auto accessorBindingIndex = static_cast<uint32>(accessorBindingMapping.m_vecAccessorBinding2.size());

			bool ok = false;
			auto& bindingTypeDecl = it0.GetBindingTypeDecl();
			if (clang_getCursorKind(bindingTypeDecl.m_cursorDecl) == CXCursor_NoDeclFound)
			{
				ok = accessorBindingMapping.m_mapBasicTypeKindToAccessorBindingIndex.insert({ bindingTypeDecl.m_CXType.kind, {accessorBindingIndex, bindingTypeDecl.m_CXType} }).second;
			}
			else
			{
				auto cursorDecl = bindingTypeDecl.m_cursorDecl;
				bool declValid = true;
				auto kind = clang_getCursorKind(cursorDecl);
				if ((kind == CXCursor_TypeAliasTemplateDecl) || (kind == CXCursor_TypeAliasDecl))
				{
					auto itFound = m_collectionData.deprecated_m_mapAliasTemplateDeclToClassTemplateCursor.find(cursorDecl);
					if (itFound != m_collectionData.deprecated_m_mapAliasTemplateDeclToClassTemplateCursor.end())
					{
						cursorDecl = itFound->second;
					}
					else
					{
						declValid = false;
					}
				}

				if (declValid)
				{
					ok = accessorBindingMapping.m_mapBasicCusorDeclToAccessorBindingIndex.insert({ bindingTypeDecl.m_cursorDecl, accessorBindingIndex }).second;
					//ok = accessorBindingMapping.m_mapBasicCusorDeclToAccessorBindingIndex.insert({ cursorDecl, accessorBindingIndex }).second;//ԭʼDecl, ��������
				}
			}
			if (!ok)
			{
				GenLogError(context.m_log, "Binding types are conflicting, an identical binding type is expected for an accessor");
				break;//todo: ����2��for, ��Ҫ��������߼�����д����򲻱���sibling
			}

			CAccessorBinding item(it0.GetAccessorTypeDecl(), it0.m_actualFieldDeclCursor, it0.m_accessorData);
			for (uint32 idx1 = 0; idx1 < it0.GetBindingTypeDeclsCount(); ++idx1)
			{
				auto& bindingTypeDecl = it0.GetBindingTypeDecl(idx1);
				item.m_vecWWWW.push_back(bindingTypeDecl);
			}
			//auto a = CXStringToCString(clang_getCursorSpelling(item.m_accessorCursorDecl));
			//printf("%s\n", a.c_str());
			accessorBindingMapping.m_vecAccessorBinding2.emplace_back(item);
		}

#ifdef TAGGED_REQUIREMENT_OF_NON_TEMPLATE_ACCESSOR_TYPE
		//todo: �ɼ�, ʵ����ֻ�з�ģ��cursor���п��ܾ���TaggedNode
		for (auto& it0 : accessorBindingMapping.m_vecAccessorBinding2)
		{
			auto ret = m_mapAccessorCursorDeclToAccessorBindingIndex.insert({ it0.m_accessorSubcursor.m_cursorDecl, &it0.m_accessorTaggedType });
			if (!ret.second)
			{
				GenLogError(context.m_log, "Accessor types are conflicting, an identical binding is expected");
				break;//todo: ����2��for, ��Ҫ��������߼�����д����򲻱���sibling
			}
			//for (auto& it1 : it0.m_vecWWWW)
			//{
			//	auto ret = m_mapAccessorCursorDeclToAccessorBindingIndex.insert({ it1.m_subcursor.m_cursorDecl, &it1.m_untaggedType });
			//	if (!ret.second)
			//	{
			//		for (auto& ssssssss : m_mapAccessorCursorDeclToAccessorBindingIndex)
			//		{
			//			auto a = CXStringToCString(clang_getCursorSpelling(ssssssss.first));
			//			printf("%s\n", a.c_str());
			//		}
			//		auto a = CXStringToCString(clang_getCursorSpelling(it1.m_subcursor.m_cursorDecl));
			//		printf("%s\n", a.c_str());

			//		GenLogError(context.m_log, "Accessor types are conflicting, an identical binding is expected");
			//		break;//todo: ����2��for, ��Ҫ��������߼�����д����򲻱���sibling
			//	}
			//}
		}
#else
#endif

		this->ResolveRecurs2(taggedRoot, context, data);

		ASSERT(m_foundCursorsCount <= accessorBindingMapping.m_vecAccessorBinding2.size());

		//for (auto& it0 : accessorBindingMapping.m_vecAccessorBinding2)
		//{
		//	auto a = CXStringToCString(clang_getCursorSpelling(it0.m_accessorCursorDecl));
		//	printf("%s\n", a.c_str());
		//}

		//δʵ�ְ�CursorDeclaration����˳�����, ��������ResolveDependcies
		for (auto& it : data.m_vecResolvedTypes2)
			it.m_taggedType->Deprecated_ResolveDependcies(data.deprecated_m_mapping.m_mapCursorDeclToTaggedType);

		//for (auto& it : data.m_mapping.m_mapCursorDeclToUntaggedTemplate)
		//{
		//	auto a = CXStringToCString(clang_getCursorSpelling(it.first));
		//	if (a == "TMyTransform")
		//		printf("");
		//	printf("%s\n", a.c_str());
		//}

		this->TestResolveRecurs3(taggedRoot, context, data);

		auto& vec0 = m_collectionData.m_accessorBindingMapping->m_settings.m_vecAccessorBindingSetting;

		for (uint32 idx0 = 0; idx0 < vec0.size(); ++idx0)
		{
			auto& bSubcursor = vec0[idx0].GetBindingTypeDecl();
			auto a = CXStringToCString(clang_getCursorSpelling(bSubcursor.m_cursorDecl));

			Niflect::CString dddddd;
			GenerateTemplateInstanceCode(bSubcursor, dddddd);
			auto ddddddd = clang_getSpecializedCursorTemplate(bSubcursor.m_cursorDecl);

			auto itFound0 = data.deprecated_m_mapping.m_mapCursorDeclToUntaggedTemplate.find(bSubcursor.m_cursorDecl);
			if (itFound0 != data.deprecated_m_mapping.m_mapCursorDeclToUntaggedTemplate.end())
			{
				auto b = CXStringToCString(clang_getCursorSpelling(itFound0->second->GetCursor()));
				printf("");
			}
			else
			{
				printf("");
			}

			//�ػ���ԭ����, ����ClassDecl��ȡ��ClassTemplate
			auto itFound1 = data.deprecated_m_mapping.m_mapCursorDeclToUntaggedTemplate.find(ddddddd);
			if (itFound1 != data.deprecated_m_mapping.m_mapCursorDeclToUntaggedTemplate.end())
			{
				auto b = CXStringToCString(clang_getCursorSpelling(itFound1->second->GetCursor()));
				printf("");
			}
			else
			{
				printf("");
			}
		}
	}
	//static void FindBindingTypeRecurs(const CAccessorBindingMapping2* mapping, const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, Niflect::TArrayNif<uint32>& vecFoundIdx, uint32& detailIteratingIdx);
	//static void IterateForTemplate(const CAccessorBindingMapping2* mapping, const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, Niflect::TArrayNif<uint32>& vecFoundIdx, uint32& detailIteratingIdx)
	//{
	//	auto argsCount = clang_Type_getNumTemplateArguments(fieldOrArgCXType);
	//	for (int32 idx1 = 0; idx1 < argsCount; ++idx1)
	//	{
	//		CXType argType = clang_Type_getTemplateArgumentAsType(fieldOrArgCXType, idx1);
	//		FindBindingTypeRecurs(mapping, argType, vecDetailCursor, vecFoundIdx, detailIteratingIdx);
	//	}
	//}
	//class CFoundResult
	//{
	//public:
	//	CFoundResult(Niflect::TArrayNif<uint32>& vecFoundIdx)
	//		: m_vecFoundIdx(vecFoundIdx)
	//		, m_foundIdx(INDEX_NONE)
	//		, m_continuing(true)
	//	{
	//	}
	//	Niflect::TArrayNif<uint32>& m_vecFoundIdx;
	//	uint32 m_foundIdx;
	//	bool m_continuing;
	//};
	//static bool FindBindingTypesSSSSSSSSSSS(const CAccessorBindingMapping2* mapping, const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, uint32& detailIteratingIdx, CFoundResult& result)
	//{
	//	auto& foundIdx = result.m_foundIdx;
	//	auto& continuing = result.m_continuing;
	//	auto& vecFoundIdx = result.m_vecFoundIdx;
	//	{
	//		auto itFound = mapping->m_mapCXTypeToIndex.find(fieldOrArgCXType);
	//		if (itFound != mapping->m_mapCXTypeToIndex.end())
	//			foundIdx = itFound->second;
	//	}
	//	if (foundIdx == INDEX_NONE)
	//	{
	//		//�ػ�, <Niflect::TArrayNif<bool>, ��ֱ��ͨ��field����CXType��cursor���ҵ�BindingType��cursor
	//		auto cursor = clang_getTypeDeclaration(fieldOrArgCXType);
	//		auto itFound = mapping->m_mapSpecializedCursorToIndex.find(cursor);
	//		if (itFound != mapping->m_mapSpecializedCursorToIndex.end())
	//		{
	//			foundIdx = itFound->second;
	//			continuing = false;
	//		}
	//	}
	//	if (foundIdx == INDEX_NONE)
	//	{
	//		const CXCursor* refCursor = NULL;
	//		while ((detailIteratingIdx++) < vecDetailCursor.size())
	//		{
	//			auto& it = vecDetailCursor[detailIteratingIdx];
	//			auto kind = clang_getCursorKind(it);
	//			//�����׸��ɴ�������͵�Ref
	//			if (kind == CXCursor_TemplateRef || kind == CXCursor_TypeRef)
	//			{
	//				refCursor = &it;
	//				break;
	//			}
	//		}
	//		if (refCursor != NULL)
	//		{
	//			auto cursor = clang_getCursorReferenced(*refCursor);
	//			auto itFound = mapping->m_mapCursorToIndex.find(cursor);
	//			if (itFound != mapping->m_mapCursorToIndex.end())
	//			{
	//				foundIdx = itFound->second;
	//				continuing = IsCursorTemplateDecl(cursor);
	//			}
	//		}
	//	}
	//	if (foundIdx != INDEX_NONE)
	//	{
	//		vecFoundIdx.push_back(foundIdx);
	//		return true;
	//	}
	//	return false;
	//}
	//static void FindBindingTypeRecurs(const CAccessorBindingMapping2* mapping, const CXType& fieldOrArgCXType, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, Niflect::TArrayNif<uint32>& vecFoundIdx, uint32& detailIteratingIdx)
	//{
	//	CFoundResult result(vecFoundIdx);
	//	if (FindBindingTypesSSSSSSSSSSS(mapping, fieldOrArgCXType, vecDetailCursor, detailIteratingIdx, result))
	//	{
	//		if (result.m_continuing)
	//		{
	//			auto& bindingSetting = mapping->m_vecAccessorBindingSetting[result.m_foundIdx];
	//			auto elemDeclsCount = bindingSetting.GetELementBindingTypeDeclsCount();
	//			if (elemDeclsCount == 0)
	//			{
	//				//ģ�����ػ�, Niflect::TArrayNif<Niflect::TArrayNif<bool> >
	//				IterateForTemplate(mapping, fieldOrArgCXType, vecDetailCursor, vecFoundIdx, detailIteratingIdx);
	//			}
	//			else
	//			{
	//				//��άBindingType
	//				for (uint32 idx0 = 0; idx0 < elemDeclsCount; ++idx0)
	//				{
	//					auto& elemSubcursor = bindingSetting.GetELementBindingTypeDecl(idx0);
	//					CFoundResult result2222(vecFoundIdx);
	//					uint32 aaaa = 0;
	//					if (!FindBindingTypesSSSSSSSSSSS(mapping, elemSubcursor.m_CXType, elemSubcursor.m_vecAaaaaaaaaa, aaaa, result2222))
	//					{
	//						ASSERT(false);//todo: ����
	//						break;
	//					}
	//					auto p = &detailIteratingIdx;
	//					auto idxForRepeatedDetail = detailIteratingIdx;
	//					if (idx0 > 0)
	//						p = &idxForRepeatedDetail;
	//					IterateForTemplate(mapping, fieldOrArgCXType, vecDetailCursor, vecFoundIdx, *p);
	//				}
	//			}
	//		}
	//	}
	//}
	//static void FindBindingTypeForField(const CAccessorBindingMapping2* mapping, const CXCursor& fieldCursor, const Niflect::TArrayNif<CXCursor>& vecDetailCursor, Niflect::TArrayNif<uint32>& vecFoundIdx)
	//{
	//	auto fieldCXType = clang_getCursorType(fieldCursor);
	//	uint32 detailIteratingIdx = 0;
	//	FindBindingTypeRecurs(mapping, fieldCXType, vecDetailCursor, vecFoundIdx, detailIteratingIdx);
	//}
	void CResolver::TestResolveRecurs3(CTaggedNode2* taggedParent, CResolvingContext& context, CResolvedData& data, int lv)
	{
		if (auto taggedType = CTaggedType::CastChecked(taggedParent))
		{
			//auto a = CXStringToCString(clang_getCursorSpelling(taggedParent->GetCursor()));
			//auto strLevel = NiflectUtil::DebugIndentToString(lv);
			//printf("%s%s\n", strLevel.c_str(), a.c_str());
		}
		else if (auto taggedType = CTaggedInheritableTypeMember::CastChecked(taggedParent))
		{
			//auto a = CXStringToCString(clang_getCursorSpelling(taggedParent->GetCursor()));
			//auto strLevel = NiflectUtil::DebugIndentToString(lv);
			//printf("%s%s\n", strLevel.c_str(), a.c_str());

			auto memberName = CXStringToCString(clang_getCursorSpelling(taggedParent->GetCursor()));
			//��Ϊ���Գ�Ա���Ͳ���BindingType
			auto underScorePos = memberName.find_last_of('_');
			if (underScorePos != std::string::npos && (underScorePos > 1))
			{
				auto& fieldCursor = taggedParent->GetCursor();
				Niflect::TArrayNif<uint32> vecFoundIdx;
				ASSERT(false);//�º����Ѹ�д
				//m_collectionData.m_accessorBindingMapping->FindBindingTypeForField(fieldCursor, taggedType->m_vecDetailCursor, vecFoundIdx);
				ASSERT(vecFoundIdx.size() > 0);
				printf("");
			}
		}

		lv++;
		for (auto& it0 : taggedParent->DebugGetChildren())
		{
			this->TestResolveRecurs3(it0.Get(), context, data, lv);
		}
	}
	void CResolver::ResolveRecurs2(CTaggedNode2* taggedParent, CResolvingContext& context, CResolvedData& data)
	{
		if (auto taggedType = CTaggedType::CastChecked(taggedParent))
		{
			auto& cursor = taggedType->GetCursor();
			ASSERT(clang_isDeclaration(clang_getCursorKind(cursor)));
			//auto ret = m_mapCusorDeclToTaggedType.insert({ cursor, taggedType });
			//if (!ret.second)
			//{
			//	GenLogError(context.m_log, "Tagged type collection goes wrong");
			//	return;//todo: ���ڵݹ�, ��Ҫ��������߼�����д����򲻱���sibling
			//}
			//bool isAccessorCursorDecl = false;
			//if (m_foundCursorsCount < data.m_vecAccessorBinding2.size())
			//{
			//	auto itFound = m_mapAccessorCursorDeclToIndex.find(cursor);
			//	if (itFound != m_mapAccessorCursorDeclToIndex.end())
			//	{
			//		auto& item = data.m_vecAccessorBinding2[itFound->second];
			//		ASSERT(item.m_accesorTaggedType == NULL);
			//		item.m_accesorTaggedType = taggedType;
			//		m_foundCursorsCount++;
			//		isAccessorCursorDecl = true;
			//	}
			//}
			//if (!isAccessorCursorDecl)
			//{
			//	data.m_vecResolvedTypes2.push_back(taggedType);
			//}

			bool isAccessorCursorDecl = false;
#ifdef TAGGED_REQUIREMENT_OF_NON_TEMPLATE_ACCESSOR_TYPE
			auto& accessorBindingMapping = data.m_mapping.m_accessorBindingMapping;
			if (m_foundCursorsCount < accessorBindingMapping.m_vecAccessorBinding2.size())
			{
				auto itFound = m_mapAccessorCursorDeclToAccessorBindingIndex.find(cursor);
				if (itFound != m_mapAccessorCursorDeclToAccessorBindingIndex.end())
				{
					ASSERT(*itFound->second == NULL);
					*itFound->second = taggedType;
					m_foundCursorsCount++;
					isAccessorCursorDecl = true;
				}
			}
#else
#endif
			if (!isAccessorCursorDecl)
			{
				data.m_vecResolvedTypes2.push_back(taggedType);
			}
			//auto a = CXStringToCString(clang_getCursorSpelling(cursor));
			//printf("%s\n", a.c_str());
			data.deprecated_m_mapping.m_mapCursorDeclToTaggedType.insert({ cursor, taggedType });
		}
		else if (auto untaggedType = CUntaggedTemplate::CastChecked(taggedParent))
		{
			auto& cursor = untaggedType->GetCursor();
			data.deprecated_m_mapping.m_mapCursorDeclToUntaggedTemplate.insert({ cursor, untaggedType });
		}

		for (auto& it0 : taggedParent->DebugGetChildren())
		{
			this->ResolveRecurs2(it0.Get(), context, data);
		}
	}
	//void CResolver::DebugFinish(const CResolvedData& data) const
	//{
	//	for (auto& it : data.m_accessorTypeTable.m_mapBuiltinTypeKind)
	//	{
	//		auto a = CXStringToCString(clang_getTypeKindSpelling(it.first));
	//		auto b = CXStringToCString(clang_getCursorSpelling(it.second->GetCursor()));
	//		printf("%s <-> %s\n", a.c_str(), b.c_str());
	//	}
	//	printf("");
	//	for (auto& it : data.m_accessorTypeTable.m_mapUserType)
	//	{
	//		auto a = CXStringToCString(clang_getCursorSpelling(it.first));
	//		auto b = CXStringToCString(clang_getCursorSpelling(it.second->GetCursor()));
	//		printf("%s <-> %s\n", a.c_str(), b.c_str());
	//	}
	//	printf("");
	//}

	void CResolver::Resolve4(CTaggedNode2* taggedRoot, CResolvingContext& context, CResolvedData& data)
	{
		data.m_accessorBindingMapping = m_collectionData.m_accessorBindingMapping;

		this->ResolveRecurs4(taggedRoot, data, data.m_taggedMapping, data.m_untaggedTemplateMapping);

		data.m_taggedMapping.Resolve();
		data.m_untaggedTemplateMapping.Init(*m_collectionData.m_aliasChain);

		SResolvedMappings mappings{ *data.m_accessorBindingMapping, data.m_taggedMapping, data.m_untaggedTemplateMapping };
		CResolvingDependenciesContext resolvingDepCtx(mappings, context.m_log);
		SResolvingDependenciesData resolvingDepData{ data.m_signatureMapping };
		//δʵ�ְ�CursorDeclaration����˳�����, ��������ResolveDependcies
		for (auto& it0 : data.m_taggedMapping.m_vecType)
		{
			bool found = false;
			for (auto& it1 : m_moduleRegInfo.m_userProvided.m_vecModuleHeader)
			{
				auto filePath = GetCursorFilePath(it0->GetCursor());
				if (it1 == filePath)
				{
					found = true;
					break;
				}
			}
			if (!found)
				continue;

			it0->ResolveDependcies(resolvingDepCtx, resolvingDepData);
		}

		//Niflect::TMap<Niflect::CString, SModuleRegIndicesAndIncludePath> mapOriginalFilePathToModuleRegIndicesAndIncPath;
		//{
		//	ASSERT(data.m_vecWriter.size() == 0);
		//	{
		//		auto& userProvided = m_moduleRegInfo.m_userProvided;
		//		for (auto& it1 : userProvided.m_vecOriginalHeader)
		//		{
		//			auto ret = mapOriginalFilePathToModuleRegIndicesAndIncPath.insert({ it1, SModuleRegIndicesAndIncludePath() });
		//			auto& item = ret.first->second;
		//			if (ret.second)
		//			{
		//				auto incPath = CIncludesHelper::ConvertToIncludePath(it1, userProvided.m_vecHeaderSearchPath);
		//				item.m_includePath_reserved = incPath;
		//				auto orgIncPathPrivateH = NiflectUtil::ReplaceFilePathExt(incPath, NiflectGenDefinition::FileExt::H, NiflectGenDefinition::FileExt::PrivateH);
		//				auto genIncPathPrivateH = NiflectUtil::ConcatPath(m_moduleRegInfo.m_typeRegBasePath, orgIncPathPrivateH);
		//				auto orgIncPathGenH = NiflectUtil::ReplaceFilePathExt(incPath, NiflectGenDefinition::FileExt::H, NiflectGenDefinition::FileExt::GenH);
		//				auto genIncPathGenH = NiflectUtil::ConcatPath(m_moduleRegInfo.m_typeRegBasePath, orgIncPathGenH);
		//				item.m_includePathPrivateHIndex = static_cast<uint32>(data.m_vecTypeRegGenFileInfo.size());
		//				data.m_vecTypeRegGenFileInfo.push_back(CTypeRegGenFileInfo(genIncPathPrivateH, genIncPathGenH));
		//			}
		//			else
		//			{
		//				ASSERT(false);//todo: һ�ε���ֻ�����һ��ModuleReg, �ƻ�����ModuleReg����, �����ModuleReg����һ������ܵ���һЩModule�������ȴ�������ȷ���
		//			}
		//		}
		//	}

		//	//auto& accessorBindingMapping = m_resolvedData.m_mapping.m_accessorBindingMapping;
		//	//for (auto& it0 : accessorBindingMapping.m_vecAccessorBinding2)
		//	//{
		//	//	if (it0.m_accessorData.m_isNotATemplate)
		//	//	{
		//	//		auto filePath = GetCursorFilePath(it0.m_accessorSubcursor.m_cursorDecl);
		//	//		auto itFound = m_mapping.m_mapOriginalFilePathToModuleRegIndicesAndIncPath.find(filePath);
		//	//		if (itFound != m_mapping.m_mapOriginalFilePathToModuleRegIndicesAndIncPath.end())
		//	//		{
		//	//			auto& item = itFound->second;
		//	//			uint32 writerIndex = static_cast<uint32>(m_vecWriter.size());
		//	//			m_mapping.m_vecTypeRegIndices.push_back(writerIndex);
		//	//			auto& privateHeaderData = vecTypeRegGenFileInfo[item.m_includePathPrivateHIndex];
		//	//			privateHeaderData.m_vecTypeRegDataIndex.push_back(writerIndex);
		//	//			ASSERT(it0.Is1D());//��֧��ģ��, ���ֻ��Ϊ1D, ��Ӧ��Binding���Ϳ���Ϊbuiltin, ����Decl�����
		//	//			STypeRegClassWritingSetting setting = { m_moduleRegInfo.m_userProvided.m_vecHeaderSearchPath, m_resolvedData.m_mapping };
		//	//			m_vecWriter.push_back(Niflect::MakeShared<CInheritableTypeRegCodeWriter_FieldAccessor>(it0.m_accessorSubcursor.m_cursorDecl, setting, it0.m_actualFieldDeclCursor, it0.m_vecWWWW[0].m_subcursor));
		//	//			m_mapping.m_vecTypeRegIncludePathPrivateHRef.push_back(&privateHeaderData.m_prevateHIncludePath);
		//	//		}
		//	//	}
		//	//}

		//	for (auto& it0 : data.m_taggedMapping.m_vecType)
		//	{
		//		auto& cursor = it0->GetCursor();
		//		auto filePath = GetCursorFilePath(cursor);
		//		//�ڴ˴���ѡʵ����Ҫ���ɵ�������Ϊ������Resolve��֮ǰ�������п��ܳ��ֵĴ���·������, �ڴ˴��������δ����, ����ͨ���粢��ʵ��һ���Ż�
		//		auto itFound = mapOriginalFilePathToModuleRegIndicesAndIncPath.find(filePath);
		//		if (itFound != mapOriginalFilePathToModuleRegIndicesAndIncPath.end())
		//		{
		//			auto& item = itFound->second;
		//			auto& privateHeaderData = data.m_vecTypeRegGenFileInfo[item.m_includePathPrivateHIndex];
		//			uint32 writerIndex = static_cast<uint32>(data.m_vecWriter.size());
		//			data.m_regMapping.m_vecTypeRegIndices.push_back(writerIndex);
		//			privateHeaderData.m_vecTypeRegDataIndex.push_back(writerIndex);
		//			STypeRegClassWritingSetting setting = { m_moduleRegInfo.m_userProvided.m_vecHeaderSearchPath, data.deprecated_m_mapping };
		//			auto writer = it0->CreateCodeWriter(setting);
		//			data.m_vecWriter.push_back(writer);
		//			data.m_regMapping.m_vecTypeRegIncludePathPrivateHRef.push_back(&privateHeaderData.m_prevateHIncludePath);
		//		}
		//	}
		//}
	}
	void CResolver::ResolveRecurs4(CTaggedNode2* taggedParent, CResolvedData& data, CTaggedTypesMapping& taggedTypesMapping, CUntaggedTemplatesMapping& untaggedTemplatesMapping)
	{
		if (auto taggedType = CTaggedType::CastChecked(taggedParent))
		{
			auto& cursor = taggedType->GetCursor();
			ASSERT(clang_isDeclaration(clang_getCursorKind(cursor)));
			auto ret = taggedTypesMapping.m_mapCursorToIndex.insert({ cursor, static_cast<uint32>(taggedTypesMapping.m_vecType.size())});
			ASSERT(ret.second);
			taggedTypesMapping.m_vecType.push_back(taggedType);
		}
		else if (auto untaggedType = CUntaggedTemplate::CastChecked(taggedParent))
		{
			auto& cursor = untaggedType->GetCursor();
			auto ret = untaggedTemplatesMapping.m_mapCursorToIndex.insert({ cursor, static_cast<uint32>(untaggedTemplatesMapping.m_vecType.size()) });
			ASSERT(ret.second);
			untaggedTemplatesMapping.m_vecType.push_back(untaggedType);
		}

		for (auto& it0 : taggedParent->DebugGetChildren())
		{
			this->ResolveRecurs4(it0.Get(), data, taggedTypesMapping, untaggedTemplatesMapping);
		}
	}

	// Ԥ���Resolve�����ǽ����������, ���������ɴ����׼������, �罫TaggedNode������˳������
	// C++�﷨�н�ö��֧��ֻ��������ʹ��, �������÷���ȫ�ޱ�Ҫ֧��
	// ����ݲ�������˳�����ɴ���, ʹ�õ�˳����ȫ�������Դ��
	//����Ϊ���ô���, ����������˳���TaggedNode����
	
	TSharedPtr<TCursorMap<CTaggedType*> > g_my_unordered_map;//�����ָ���Ϊ����δʹ��ʱռ�õ��ڴ�(��̬ռ��)
	void DDDDDDDDDDDDDDDDDRecurs(CTaggedNode2* taggedParent, int level)
	{
		if (g_my_unordered_map == NULL)
			g_my_unordered_map = MakeShared<TCursorMap<CTaggedType*> >();

		auto& my_unordered_map = *g_my_unordered_map;
		if (auto taggedType = CTaggedType::CastChecked(taggedParent))
			my_unordered_map[taggedType->GetCursor()] = taggedType;


		//auto strLevel = NiflectUtil::DebugIndentToString(level);

		//auto& cursor = taggedParent->DebugGetCursor();
		//auto strName = CXStringToCString(clang_getCursorSpelling(cursor));
		//auto type = clang_getCursorType(cursor);
		//auto strType = CXStringToCString(clang_getTypeSpelling(type));
		//printf("%s%s, %s\n", strLevel.c_str(), strName.c_str(), strType.c_str());


		level++;

		for (auto& it0 : taggedParent->DebugGetChildren())
		{
			DDDDDDDDDDDDDDDDDRecurs(it0.Get(), level);
		}
	}
	void DDDDDDDDDDDDDDDDDRecurs2(CTaggedNode2* taggedParent, TArrayNif<CTaggedType*>& vecOrderedType, TSet<CTaggedType*>& setAdded, int level)
	{
		auto& my_unordered_map = *g_my_unordered_map;
		auto taggedType = CTaggedType::CastChecked(taggedParent);
		if (taggedType != NULL)
		{
			if (!setAdded.insert(taggedType).second)
				return;
		}
		else
		{
		}

		{
			auto& cursor = taggedParent->GetCursor();
			auto type = clang_getCursorType(cursor);
			type = GetPointerOriginalType(type);
			auto typeCursor = clang_getTypeDeclaration(type);
			auto itFound = my_unordered_map.find(typeCursor);
			if (itFound != my_unordered_map.end())
			{
				DDDDDDDDDDDDDDDDDRecurs2(itFound->second, vecOrderedType, setAdded, level);
			}
			else
			{
				//root��builtin
			}
		}

		for (auto& it0 : taggedParent->DebugGetChildren())
		{
			DDDDDDDDDDDDDDDDDRecurs2(it0.Get(), vecOrderedType, setAdded, level);
		}

		if (taggedType != NULL)
		{
			//vecOrderedType.insert(vecOrderedType.begin(), taggedType);
			vecOrderedType.push_back(taggedType);
		}
	}
	void SortTaggedNodesInDependencyOrder(CTaggedNode2* taggedParent, Niflect::TArrayNif<CTaggedType*>& vecOrderedType)
	{
		DDDDDDDDDDDDDDDDDRecurs(taggedParent, 0);
		TSet<CTaggedType*> setAdded;
		DDDDDDDDDDDDDDDDDRecurs2(taggedParent, vecOrderedType, setAdded, 0);
	}
}