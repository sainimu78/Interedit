#include "NiflectGen/Test/TestGen.h"
#include "NiflectGen/Generator/Generator.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Collector/Collector.h"
#include "NiflectGen/Resolver/Resolver.h"

namespace TestGen
{
	using namespace NiflectGen;

	static void TestSuccess_AccessorBindingWithAlias()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestAccessorBindingAliass.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([](void* cursorAddr)
				{
					auto& cursor = *static_cast<CXCursor*>(cursorAddr);
					CTaggedNode2 taggedRoot;
					CGenLog log;
					log.Config(CGenLogOption().SetAssertionOnAddingItem(false).SetCachedItems(true));
					CCollectingContext context(&log);
					CCollectionData collectionData;
					CDataCollector collector;
					collector.Collect(cursor, &taggedRoot, context, collectionData);
					auto& vec = collectionData.m_accessorBindingMapping->m_vecAccessorBindingSetting;
					ASSERT(log.m_vecText.size() == 0);
					//ASSERT(vec.size() == 1);
					//auto accessorTypeName = CXStringToCString(clang_getTypeSpelling(vec.back().m_subcursorRoot.m_vecChild[0].m_CXType));
					//ASSERT(accessorTypeName == "TMyAccessorAlias<T>");
					//auto bindingTypeName = CXStringToCString(clang_getTypeSpelling(vec.back().m_subcursorRoot.m_vecChild[1].m_CXType));
					//ASSERT(bindingTypeName == "TestAccessor2::TMyTransform<float>");
				});
		}
	}
	static void TestSuccess_BindingTypesAllUnique()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestBindingTypesNoDup.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([](void* cursorAddr)
				{
					auto& cursor = *static_cast<CXCursor*>(cursorAddr);
					CTaggedNode2 taggedRoot;
					CGenLog log;
					CCollectingContext context(&log);
					CCollectionData collectionData;
					CDataCollector collector;
					collector.Collect(cursor, &taggedRoot, context, collectionData);
					auto& vec = collectionData.m_accessorBindingMapping->m_vecAccessorBindingSetting;
					ASSERT(log.m_vecText.size() == 0);
				});
		}
	}
	static void TestFailure_BindingTypesDuplicated()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestBindingTypesDup.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([](void* cursorAddr)
				{
					auto& cursor = *static_cast<CXCursor*>(cursorAddr);
					CTaggedNode2 taggedRoot;
					CGenLog log;
					log.Config(CGenLogOption().SetAssertionOnAddingItem(false).SetCachedItems(true));
					CCollectingContext context(&log);
					CCollectionData collectionData;
					CDataCollector collector;
					collector.Collect(cursor, &taggedRoot, context, collectionData);
					auto& vec = collectionData.m_accessorBindingMapping->m_vecAccessorBindingSetting;
					ASSERT(log.m_vecText.size() == 4);
				});
		}
	}
	static void TestSuccess_FullScopes()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestBindingTypesFullScopes.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([](void* cursorAddr)
				{
					auto& cursor = *static_cast<CXCursor*>(cursorAddr);
					CTaggedNode2 taggedRoot;
					CGenLog log;
					CCollectingContext context(&log);
					CCollectionData collectionData;
					CDataCollector collector;
					collector.Collect(cursor, &taggedRoot, context, collectionData);
					ASSERT(log.m_vecText.size() == 0);
					auto& vec0 = collectionData.m_accessorBindingMapping->m_vecAccessorBindingSetting;
					ASSERT(vec0.size() > 0);

					Niflect::TArrayNif<Niflect::CString> vecExpected;
					vecExpected.push_back("EngineTypeBindingSettingScope::CMyTM");
					vecExpected.push_back("Niflect::TArrayNif<float>");
					vecExpected.push_back("Niflect::TArrayNif<EngineTypeBindingSettingScope::CMyTM>");
					vecExpected.push_back("TestGenMyScope::MyAliasInt8_0");
					vecExpected.push_back("TestGenMyScope::MyAliasInt8_1");
					vecExpected.push_back("Niflect::TMap<Niflect::CString, EngineTypeBindingSettingScope::CMyTM>");
					vecExpected.push_back("Niflect::TMap<float, EngineTypeBindingSettingScope::CMyTM>");
					vecExpected.push_back("Niflect::TMap<bool, EngineTypeBindingSettingScope::CMyTM>");
					vecExpected.push_back("Niflect::TMap<float, bool>");
					vecExpected.push_back("Niflect::TMap<std::string, bool>");
					vecExpected.push_back("std::map<std::string, int8>");
					vecExpected.push_back("TestGenMyScope::MyAliasInt8_2");
					vecExpected.push_back("TestGenMyScope::SubScope::SubMyAliasInt8_0");
#ifdef CLASS_TYPE_SCOPE
					vecExpected.push_back("TestGenMyScope::CSub_0::CSubSub_0");
					vecExpected.push_back("TestGenMyScope::CSub_1::CSubSub_0");
#else
#endif
					vecExpected.push_back("TestGenMyScope::MyAliasInt8_3");
					vecExpected.push_back("TestGenMyScope::MyAliasInt8_4");
					vecExpected.push_back("TestGenMyScope::MyAliasSub_0");
					vecExpected.push_back("TestGenMyScope::MyAliasSub_1");
					vecExpected.push_back("TestGenMyScope::MyAliasSub_2");
#ifdef TEMPLATE_INSTANCE_SCOPE
					vecExpected.push_back("TestGenMyScope::TSub_2<float>::CSubSub_0");
#endif

					uint32 testedIdx = 0;
					for (uint32 idx0 = 0; idx0 < vec0.size(); ++idx0)
					{
						auto& bSubcursor = vec0[idx0].GetBindingTypeDecl();

						//auto kind = clang_getCursorKind(bSubcursor.m_cursorDecl);
						//if (!IsCursorKindTemplateDecl(kind))//��ģ����ݲ��ڷ�Χ, ����ҪӦȷ��ģ������ų�����namespace����(������֧��, δ����)
						//{
						//	Niflect::CString resolvedName;
						//	if (kind == CXCursor_ClassDecl)
						//	{
						//		//�ػ�ģ��
						//		if (clang_Type_getNumTemplateArguments(bSubcursor.m_CXType) > 0)
						//		{
						//			GenerateTemplateInstanceCode(bSubcursor, resolvedName, CGenerateTemplateInstanceCodeOption().SetWithFullScope(true));
						//		}
						//		else
						//		{
						//			//����Scope�е�����, �� TestGenMyScope::CSub::CSubSub
						//			resolvedName += GenerateNamespacesAndScopesCode(bSubcursor.m_cursorDecl);
						//			resolvedName += CXStringToCString(clang_getCursorSpelling(bSubcursor.m_cursorDecl));
						//		}
						//	}
						//	else
						//	{
						//		if ((kind == CXCursor_TypeAliasDecl)//using ����
						//			|| (kind == CXCursor_TypedefDecl)//typedef ����
						//			)
						//		{
						//			resolvedName += GenerateNamespacesAndScopesCode(bSubcursor.m_cursorDecl);
						//			resolvedName += CXStringToCString(clang_getCursorSpelling(bSubcursor.m_cursorDecl));
						//		}
						//		else
						//		{
						//			//Builtin����
						//			ASSERT(kind == CXCursor_NoDeclFound);
						//			resolvedName += CXStringToCString(clang_getTypeSpelling(bSubcursor.m_CXType));
						//		}
						//	}
						//	ASSERT(vecExpected[testedIdx] == resolvedName);
						//	testedIdx++;
						//}

						auto instanceTypeName = GenerateFullScopeTypeName(bSubcursor);
						ASSERT(vecExpected[testedIdx] == instanceTypeName);
						testedIdx++;
					}
					ASSERT(testedIdx == vecExpected.size());
				});
		}
	}
	static void TestSuccess_FieldsFinding()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecOriginalHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestOriginalFieldsFinding.h"));
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestBindingTypesFieldsFinding.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([](void* cursorAddr)
				{
					auto& cursor = *static_cast<CXCursor*>(cursorAddr);
					CTaggedNode2 taggedRoot;
					CGenLog log;
					CCollectingContext context(&log);
					CCollectionData collectionData;
					CDataCollector collector;
					collector.Collect(cursor, &taggedRoot, context, collectionData);
					ASSERT(log.m_vecText.size() == 0);
					CResolvingContext resolvingContext(&log);
					CResolver resolver(collectionData);
					CResolvedData resolvedData;
					resolver.Resolve2(&taggedRoot, resolvingContext, resolvedData);
					ASSERT(log.m_vecText.size() == 0);
				});
		}
	}
	void TestCollector()
	{
		//TestSuccess_AccessorBindingWithAlias();
		//TestSuccess_BindingTypesAllUnique();
		//TestFailure_BindingTypesDuplicated();
		//TestSuccess_FullScopes();
		TestSuccess_FieldsFinding();
	}
}