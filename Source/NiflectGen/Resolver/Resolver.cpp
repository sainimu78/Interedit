#include "NiflectGen/Resolver/Resolver.h"
#include "Niflect/Util/DebugUtil.h"
#include <algorithm>
#include "Niflect/Util/TypeUtil.h"

namespace NiflectGen
{
	using namespace Niflect;

	CResolver::CResolver(const CCollectionData& collectionData, const Niflect::TArrayNif<Niflect::CString>& vecHeaderSearchPath)
		: m_collectionData(collectionData)
		//, m_vecHeaderSearchPath(vecHeaderSearchPath)
		, m_foundCursorsCount(0)
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
	void CResolver::Resolve2(CTaggedNode2* taggedRoot, CResolvingContext& context, CResolvedData& data)
	{
		auto& accessorBindingMapping = data.m_mapping.m_accessorBindingMapping;
		for (auto& it0 : m_collectionData.m_vecBindingSetting)
		{
			if (!it0.IsValidBindingSetting())
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
					auto itFound = m_collectionData.m_mapAliasTemplateDeclToClassTemplateCursor.find(cursorDecl);
					if (itFound != m_collectionData.m_mapAliasTemplateDeclToClassTemplateCursor.end())
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
			for (uint32 idx1 = 0; idx1 < it0.GetDimensionalBindingTypeDeclsCount(); ++idx1)
			{
				auto& bindingTypeDecl = it0.GetDimensionalBindingTypeDecl(idx1);
				item.m_vecWWWW.push_back(bindingTypeDecl);
			}
			//auto a = CXStringToCString(clang_getCursorSpelling(item.m_accessorCursorDecl));
			//printf("%s\n", a.c_str());
			accessorBindingMapping.m_vecAccessorBinding2.emplace_back(item);
		}

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

		this->ResolveRecurs2(taggedRoot, context, data);

		ASSERT(m_foundCursorsCount <= accessorBindingMapping.m_vecAccessorBinding2.size());

		//for (auto& it0 : accessorBindingMapping.m_vecAccessorBinding2)
		//{
		//	auto a = CXStringToCString(clang_getCursorSpelling(it0.m_accessorCursorDecl));
		//	printf("%s\n", a.c_str());
		//}

		//δʵ�ְ�CursorDeclaration����˳�����, ��������ResolveDependcies
		for (auto& it : data.m_vecResolvedTypes2)
			it.m_taggedType->ResolveDependcies(data.m_mapping.m_mapCursorDeclToTaggedType);
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

			auto& accessorBindingMapping = data.m_mapping.m_accessorBindingMapping;
			bool isAccessorCursorDecl = false;
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
			if (!isAccessorCursorDecl)
			{
				data.m_vecResolvedTypes2.push_back(taggedType);
			}
			//auto a = CXStringToCString(clang_getCursorSpelling(cursor));
			//printf("%s\n", a.c_str());
			data.m_mapping.m_mapCursorDeclToTaggedType.insert({ cursor, taggedType });
		}
		else if (auto untaggedType = Niflect::CastChecked<CUntaggedTemplate>(taggedParent))
		{
			auto& cursor = untaggedType->GetCursor();
			data.m_mapping.m_mapCursorDeclToUntaggedTemplate.insert({ cursor, untaggedType });
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