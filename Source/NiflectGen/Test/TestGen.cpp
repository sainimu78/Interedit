#include "NiflectGen/Test/TestGen.h"
#include "NiflectGen/Generator/Generator.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Collector/Collector.h"
#include "NiflectGen/Resolver/Resolver.h"
#include "NiflectGen/Test/TestTypeRegSignature.h"
#include "NiflectGen/CodeWriter/ModuleReg/ModuleRegCodeWriter.h"

namespace TestGen
{
	using namespace NiflectGen;

	static void TestSuccess_AccessorAlias()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestAccessorBindingAccessorTypeAliass.h"));
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
					vecExpected.push_back("TestAccessor2::TMyTransform<float>");
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
	static void TestSuccess_AccessorFinding()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecOriginalHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestOriginalTypeAccessorFinding.h"));
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestAccessorBindingAccessorFinding.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([&info](void* cursorAddr)
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
					CModuleRegInfoValidated validatedModuleRegInfo(info);
					CResolver resolver(collectionData, validatedModuleRegInfo);
					CResolvedData resolvedData;
					resolver.Resolve4(&taggedRoot, resolvingContext, resolvedData);
					ASSERT(log.m_vecText.size() == 0);
					Niflect::TArrayNif<Niflect::CString> vecExpected;
					vecExpected.push_back("CCompoundAccessor");//��0����ӦΪö�ٵ�Accessor, ��δʵ�ֶ�Ӧ��AccessorBinding, ʵ�ֺ���������Ӧ�Ĳ���
					vecExpected.push_back("MyAccessor::CMyClassAccessor");
					vecExpected.push_back("CCompoundAccessor");
					uint32 idx0 = 0;
					//���� TaggedType ���в���
					for (auto& it0 : resolvedData.m_signatureMapping.m_vecItem)
					{
						if (it0.m_resoRoot.m_taggedTypeIndex != INDEX_NONE)
						{
							Niflect::CString accessorResocursorName;
							if (it0.m_resoRoot.m_accessorBindingIndex != INDEX_NONE)
							{
								auto& setting = resolvedData.m_accessorBindingMapping->m_vecAccessorBindingSetting[it0.m_resoRoot.m_accessorBindingIndex];
								accessorResocursorName = setting.m_accessorTypeCursorName;
							}
							else
							{
								accessorResocursorName = NiflectGenDefinition::NiflectFramework::AccessorTypeName::CompoundField;
							}
							ASSERT(vecExpected[idx0++] == accessorResocursorName);
						}
					}
					ASSERT(vecExpected.size() == idx0);
				});
		}
	}
	static void TestSuccess_RequiredHeader()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecOriginalHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestOriginalRequiredHeader.h"));
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestAccessorBindingRequiredHeader.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([&info](void* cursorAddr)
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
					CModuleRegInfoValidated validatedModuleRegInfo(info);
					CResolver resolver(collectionData, validatedModuleRegInfo);
					CResolvedData resolvedData;
					resolver.Resolve4(&taggedRoot, resolvingContext, resolvedData);
					ASSERT(log.m_vecText.size() == 0);
					Niflect::TArrayNif<Niflect::CString> vecExpected;
					vecExpected.push_back("NiflectGen/Test/TestSomeTypes.h");
					vecExpected.push_back("NiflectGen/Test/TestOriginalRequiredHeader.h");//�౾��indexedRoot����ͷ�ļ�
					vecExpected.push_back("Niflect/NiflectBase.h");
					vecExpected.push_back("Engine/Test/TestMyTransform.h");
					vecExpected.push_back("BypassCode/NiflectSTL/string");
					vecExpected.push_back("NiflectGen/Test/TestOriginalRequiredHeader.h");//�Զ���TMyArray����ͷ�ļ�
					uint32 idxExpected = 0;
					for (auto& it0 : resolvedData.m_signatureMapping.m_vecItem)
					{
						if (it0.m_resoRoot.m_vecRequiredHeaderFilePath.size() > 0)
						{
							for (auto& it1 : it0.m_resoRoot.m_vecRequiredHeaderFilePath)
							{
								auto pos = it1.find(vecExpected[idxExpected]);
								ASSERT(pos != std::string::npos);
								idxExpected++;
								//printf("%s\n", it1.c_str());
							}
							//printf("################\n");
						}
					}
					ASSERT(idxExpected == vecExpected.size());
				});
		}
	}
	static void TestSuccess_ResocursorName()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_vecOriginalHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestOriginalTypeResocursorName.h"));
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestAccessorBindingResocursorName.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([&info](void* cursorAddr)
				{
					auto& cursor = *static_cast<CXCursor*>(cursorAddr);
					CTaggedNode2 taggedRoot;
					CGenLog log;
					CCollectingContext context(&log);
					CCollectionData collectionData;
					CDataCollector collector;
					collector.Collect(cursor, &taggedRoot, context, collectionData);
					ASSERT(log.m_vecText.size() == 0);
					Niflect::TArrayNif<Niflect::CString> vecExpectedA;
					Niflect::TArrayNif<Niflect::CString> vecExpectedB;
					vecExpectedA.push_back("TestAccessor2::TMyTransformAccessor<float>");
					vecExpectedB.push_back("TestAccessor2::TMyTransform<float>");
					vecExpectedA.push_back("TestAccessor2::TMyTransformAccessor<float>");
					vecExpectedB.push_back("TestAccessor2::TMyTransform<double>");
					vecExpectedA.push_back("TestAccessor2::TMyTransformAccessor<float>");
					vecExpectedB.push_back("MyScope2::TMyScope2TM<int32>");
					vecExpectedA.push_back("TestAccessor2::TMyTransformAccessor<float>");
					vecExpectedB.push_back("MyScope2::SubScope1::TMySubScope1TM<uint32>");
					vecExpectedA.push_back("TestAccessor2::TMyTransformAccessor<float>");
					vecExpectedB.push_back("MyScope2::SubScope1::TMySubScope1TM<int64>");
					vecExpectedA.push_back("TestAccessor2::TMyTransformAccessor<float>");
					vecExpectedB.push_back("Niflect::TArrayNif<MyScope2::SMyStruct>");
					vecExpectedA.push_back("Engine::TStlArrayAccessor");
					vecExpectedB.push_back("Niflect::TArrayNif");
					vecExpectedA.push_back("Niflect::CCompoundAccessor");
					vecExpectedB.push_back("std::pair");
					uint32 idxA = 0;
					uint32 idxB = 0;
					auto& vecCollected = collectionData.m_accessorBindingMapping->m_vecAccessorBindingSetting;
					for (auto& it : vecCollected)
					{
						ASSERT(it.m_accessorTypeCursorName == vecExpectedA[idxA++]);
						ASSERT(it.m_bindingTypeCursorName == vecExpectedB[idxB++]);
					}
					ASSERT(idxA == vecCollected.size());
					ASSERT(idxB == vecCollected.size());
				});
		}
	}
	static void TestSuccess_TypeRegCodeGen()
	{
		auto memTest = Niflect::GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			CModuleRegInfo info;
			info.m_moduleName = "Engine";
			info.m_genIncludeBasePath = "NiflectGenerated";
			info.m_genBasePath = "F:/Fts/Proj/Test/Interedit/Generated";
			info.m_vecOriginalHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestOriginalTypeTypeRegCodeGen.h"));
			info.m_vecBindingSettingHeader.push_back(CONCAT_CONST_CHAR_2(ROOT_TEST_PATH, "/TestAccessorBindingTypeTypeRegCodeGen.h"));
			NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
			gen->SetModuleRegInfo(info);
			gen->Generate([&info](void* cursorAddr)
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
					CModuleRegInfoValidated validatedModuleRegInfo(info);
					CResolver resolver(collectionData, validatedModuleRegInfo);
					CResolvedData resolvedData;
					resolver.Resolve4(&taggedRoot, resolvingContext, resolvedData);
					ASSERT(log.m_vecText.size() == 0);
					CTemplateBasedCppWriter writer(resolvedData, validatedModuleRegInfo);
					CWritingContext writingContext(&log);
					CCodeGenData writtenData;
					writer.Write3(writingContext, writtenData);
				});
		}
	}
	void TestCollector()
	{
		//TestSuccess_AccessorAlias();
		//TestSuccess_BindingTypesAllUnique();
		//TestFailure_BindingTypesDuplicated();
		//TestSuccess_FullScopes();
		//TestSuccess_TypeRegSignature();
		//TestSuccess_AccessorFinding();
		//TestSuccess_ResocursorName();
		//TestSuccess_RequiredHeader();
		TestSuccess_TypeRegCodeGen();
	}
}