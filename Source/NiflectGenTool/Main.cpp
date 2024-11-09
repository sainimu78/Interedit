#include <stdio.h>
#include "NiflectGen/Test/TestParseCpp.h"
#include "NiflectGen/Generator/Generator.h"
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Util/DebugUtil.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Test/TestGen.h"
#include "NiflectGen/Generator/GenData.h"
#include <cstring>

//�����е�����
//1. StaticRegStage, �ھ�̬��ʼ���׶ε�ע�����
//2. InitialRegStage, ���߼���ʼ��(��App�������ϵĳ�ʼ��)�׶ε�ע�����
//3. TypeReg, ��������Ͷ�Ӧ�ķ������ɴ����ע����Ϣ, ��һ��class����ʾ, ����classҲ����TypeRegClass
//4. UntaggedReg, Builtin����, ģ�����͵ķ������ɴ���, ֻ������ģ���ڲ�
//5. ModuleReg, ����ģ�����з������ɴ����ע����Ϣ
//6. SplittedModuleReg, Ϊ���ö��̱߳���, ModuleReg���ܱ��ֳɶ��ע����Ϣ, �ϲ�����ע�ᵽԭModuleReg��
//7. Accessor, ����Field�����л���ʽ, ��ƶ�д��ʽ
//8. FieldLayout, ��Accessor���ɵ����ͽṹ, ��ʾNative�������б���ǳ�Ա����(Field)�Ķ�д��ʽ
//9. AccessorBinding, Field���Ͷ�Ӧ��Accessor��, �������ɴ���ʱָ����Ӧ��Accessor
//10. NiflectType, ��ʾ����ʱ�������Ϣ, �����������Ϣ��, ��ͨ������Ϣ�����Ӧ��Native����
//11. Natimeta, ȫ��Ϊ Native Meta, ͨ��C++ Native������Meta��Ϣ, �����ڶ�����ķ�����Ϣ, ���Եķ�����Ϣ
//12. ContainerTemplate1D, 1ά����ģ��, ���������߼��ϵ�ģ�����ֻ��1��, ��std::vector, std::shared_ptr, ����std::map, �ڴ��������߼���std::pair��Ϊ��ģ�����, ����first��second��Ϊģ�����, ���std::map�ڴ��������߼���Ҳ����1ά����ģ��
//13. StructuralTemplateND, Nά�ṹģ��, ����N>=2, Ϊ����"ά"��������, Ҳ�ɳ���������ṹģ��, ÿ��ģ�������Ӧһ��Field, ��std::pair
//14. TAccesorBinding, Accessor��������͵İ�����, ����ģ��İ�, 1άBindingType�϶�ΪContainerTemplate1D, ����1ά��BindingType�����һ��BindingType�϶�ΪStructuralTemplateND, �����϶�ΪContainerTemplate1D
//15. CursorName, ��ʾ Full Scope ��ʽ�� Cursor ����, ���� Cursor ��ָ AccessorBinding �е� Cursor �� Field Cursor. ����ȫ�ػ�ģ��, ��ʾ�������� Niflect::TArray<float>, TestAccessor2::TMyTransform<float>; ���ں�ģ�������ģ��, ��ʾ�������� Engine::TStlArrayAccessor, std::pair; ���޼ƻ�֧�ֲ����ػ�����ʽ. 
//	1. CursorName Ϊ�Զ������, ���������Ϊ CursorSpelling, CursorSpelling ��һ��Ϊ Full Scope. ������� Builtin ����, �޶�Ӧ��Ч Cursor, ������Ҫͨ�� CXType ��ȡ, ��ͨ�� AccessorBinding ���ɵ� CursorName ���Ϊ Builtin ��������
//16. ResocursorName, ȫ��Ϊ Resolved Cursor Name, ͨ�� CursorName ƴ�������ػ�ģ����������͵�����, �� Niflect::TArray<float> ���а��� Niflect::TArray �� float 2 �� CursorName, ͨ���ػ�ģ�� Cursor �Ľ⹹�ݹ�ƴ������ Niflect::TArray<float>

//���ɹ���
//1. AccessorBinding ���ù���
//	1. AccessorType
//		1. ����� Niflect::CAccessor �̳�
//		2. ���嶨����ʽ
//			1. ��Ϊ class, �� CFloatAccessor
//			2. ��Ϊ1������ģ����, �� TStlArrayAccessor<TInstance>
//			3. ��Ϊģ���ػ�, �� TMyTransformAccessor<float>
//		3. ������ʽ2, ���ɴ���ʱ, �� BindingType ���������, ���ɴ��� Accessor �Ĵ����� TStlArrayAccessor<TArray<float> >
//	2. BindingType
//		1. ���嶨����ʽ
//			1. ��Ϊ Builtin ����, �� float
//			2. ��Ϊ�������(class), �ṹ��(struct), ö��(enum class)
//			3. ��Ϊ���������������ģ���ṹ��ģ��
//			4. ��Ϊģ���ػ�, �� std::vector<bool>
//			5. ��Ϊ����ģ���ػ�, �� Niflect::TArray<T>, ���� Allocator Ϊ�����ػ�, ��˲��϶��� std::vector �ظ�ָ��, ʵ������ Runtime �� typeid(Niflect::TArray<T>).hash_code() �� typeid(std::vector<float>).hash_code() �ǲ�ͬ��
//		2. ������ʽ2, Ҫ���Ӧ������ָ�����(NIF_T)
//		3. ������ʽ3, 1��������ģ�彫�϶�Ϊ����ģ��, ����1��������ģ�彫�϶�Ϊ�ṹģ��(StructuralTemplateND)
//			1. ���ڽṹģ��, Ҫ��ʵ�ʶ���ĳ�Ա������ scope ȫΪ public, �� std::pair
//		4. ��֧�ֵ���ʽΪ, ����ָ�������ͬ�� BindingType, ������ͬ��ָ Runtime �� typeid(TypeA).hash_code() == typeid(TypeB).hash_code() ����, ��������
//			1. ����ָ����ȫ��ͬ�� BindingType, �� float �� float
//			2. ����ͨ�������ֱ�ָ��ԭʼ������ͬ�� BindingType, �� float �� MyFloat0, MyFloat0 �� MyFloat1
//				1. ���� MyFloat0 Ϊ using MyFloat0 = float;
//				2. ���� MyFloat1 Ϊ using MyFloat1 = MyFloat0;
//			3. ����ͨ������ģ���ػ�����Ӧģ��ʵ����ȫ��ͬ�� BindingType, �� std::vector<T> �� MyVector0<T>
//				1. ���� MyVector0 Ϊ template <typename T> using MyVector0 = std::vecotr<T, MyAllocator0<T> >;
//					1. ���� MyAllocator0 Ϊ template <typename T> using MyAllocator0 = std::allocator<T>;
//			4. ���ɶ��岿��ģ���ػ���Ӧģ��ʵ������ȫ��ͬ�� BindingType, �� �� std::vector<T> �� MyVector1<T>
//				1. ���� MyVector1 Ϊ template <typename T> using MyVector1 = std::vecotr<T, MyAllocator1<T> >;
//					1. ���� MyAllocator1 Ϊ template <typename T> class MyAllocator1 { ... };


namespace NiflectGen
{
	//static bool FindOptionName(const Niflect::CString& str, const Niflect::CString& name, size_t& pos)
	//{
	//	pos = str.find(name);
	//	bool found = pos != std::string::npos;
	//	if (found)
	//		pos += name.length();
	//	return found;
	//}
	//static void GetAndAddOptionValue(const Niflect::CString& str, const size_t& pos, Niflect::TArrayNif<Niflect::CString>& vec)
	//{
	//	vec.push_back(str.substr(pos, str.length() - pos));
	//}
	//static void GetOptionValue(const Niflect::CString& str, const size_t& pos, Niflect::CString& out)
	//{
	//	out = str.substr(pos, str.length() - pos);
	//}

	//static void ParseOptions(int argc, const char** argv, CModuleRegInfo& info)
	//{
	//	ASSERT(argc > 1);
	//	for (int idx = 1; idx < argc; ++idx)
	//	{
	//		Niflect::CString strV = argv[idx];
	//		if (strV.length() >= 2)
	//		{
	//			size_t pos;
	//			if (FindOptionName(strV, "-n", pos))
	//			{
	//				GetOptionValue(strV, pos, info.m_moduleName);
	//			}
	//			else if (FindOptionName(strV, "-h", pos))
	//			{
	//				GetAndAddOptionValue(strV, pos, info.m_vecModuleHeader);
	//			}
	//			else if (FindOptionName(strV, "-am", pos))
	//			{
	//				GetOptionValue(strV, pos, info.m_moduleApiMacro);
	//			}
	//			else if (FindOptionName(strV, "-mh", pos))
	//			{
	//				GetOptionValue(strV, pos, info.m_moduleApiMacroHeader);
	//			}
	//			else if (FindOptionName(strV, "-s", pos))
	//			{
	//				GetAndAddOptionValue(strV, pos, info.m_vecAccessorSettingHeader);
	//			}
	//			else if (FindOptionName(strV, "-I", pos))
	//			{
	//				GetAndAddOptionValue(strV, pos, info.m_vecModuleHeaderSearchPath);
	//			}
	//			else if (FindOptionName(strV, "-p", pos))
	//			{
	//				GetOptionValue(strV, pos, info.m_outputRootPath_genIncludeSearchPath);
	//			}
	//			else if (FindOptionName(strV, "-fs", pos))
	//			{
	//				info.m_genFileMode = EGeneratingHeaderAndSourceFileMode::ESourceAndHeader;
	//			}
	//			else
	//			{
	//				ASSERT(false);
	//			}
	//		}
	//	}
	//}

	static Niflect::CString GetNextArgValue(const char** argv, int& idx)
	{
		idx += 1;
		Niflect::CString str;
		auto& psz = argv[idx];
		if (psz[0] == '\"')
		{
			str = &psz[1];
			ASSERT(str.length() >= 2);
			if (str.back() == '\"')
				str.erase(str.begin() + str.length() - 1);
			else
				ASSERT(false);
		}
		else
		{
			str = psz;
		}
		return str;
	}

	static void ParseOptions(int argc, const char** argv, CModuleRegInfo& info)
	{
		ASSERT(argc > 1);
		for (int idx = 1; idx < argc; ++idx)
		{
			auto& pszV = argv[idx];
			if (strcmp(pszV, "-n") == 0)
			{
				info.m_moduleName = GetNextArgValue(argv, idx);
			}
			else if (strcmp(pszV, "-h") == 0)
			{
				info.m_vecModuleHeader.push_back(GetNextArgValue(argv, idx));
			}
			else if (strcmp(pszV, "-am") == 0)
			{
				info.m_moduleApiMacro = GetNextArgValue(argv, idx);
			}
			else if (strcmp(pszV, "-amh") == 0)
			{
				info.m_moduleApiMacroHeader = GetNextArgValue(argv, idx);
			}
			else if (strcmp(pszV, "-a") == 0)
			{
				info.m_vecAccessorSettingHeader.push_back(GetNextArgValue(argv, idx));
			}
			else if (strcmp(pszV, "-I") == 0)
			{
				info.m_vecModuleHeaderSearchPath.push_back(GetNextArgValue(argv, idx));
			}
			else if (strcmp(pszV, "-p") == 0)
			{
				info.m_outputRootPath_genIncludeSearchPath = GetNextArgValue(argv, idx);
			}
			else if (strcmp(pszV, "-fs") == 0)
			{
				info.m_genFileMode = EGeneratingHeaderAndSourceFileMode::ESourceAndHeader;
			}
			else
			{
				ASSERT(false);
			}
		}
	}

}

int main(int argc, const char** argv)
{
	if (false)//if (true)//
	{
		TestGen::TestCollector();
	}

	if (true)//if (false)//
	{
		using namespace NiflectGen;

		using namespace Niflect;
		//auto table = CreateTypeBindingTable();
		auto memTest = GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			if (true)
			{
				CModuleRegInfo info;
				
				//const char* argvTest[] = {
				//	"Placeholder",
				//	"-n", "Engine",
				//	"-h", "\"F:/Fts/Proj/Test/Interedit/Source/Engine/EngineObject.h\"",
				//	"-h", "F:/Fts/Proj/Test/Interedit/Source/Engine/DerivedObject.h",
				//	"-am", "ENGINE_API",
				//	"-amh", "F:/Fts/Proj/Test/Interedit/Source/Engine/EngineCommon.h",
				//	"-a", "F:/Fts/Proj/Test/Interedit/Source/Engine/EngineAccessorSetting.h",
				//	"-I", "F:/Fts/Proj/Test/Interedit/Source",
				//	"-p", "F:/Fts/Proj/Test/Interedit/Generated/NiflectGenerated",
				//};
				//argc = sizeof(argvTest) / sizeof(const char*);
				//argv = argvTest;

				//const char* argvTest[] = {
				//	"Placeholder",
				//	"-n", "TestModule1",
				//	"-h", "F:/Fts/Proj/Test/Interedit/Source/TestModule1/TestModule1.h",
				//	"-am", "TESTMODULE1_API",
				//	"-amh", "F:/Fts/Proj/Test/Interedit/Source/TestModule1/TestModule1Common.h",
				//	"-a", "F:/Fts/Proj/Test/Interedit/Source/Niflect/CommonlyUsed/DefaultAccessorSetting.h",
				//	"-I", "F:/Fts/Proj/Test/Interedit/Source",
				//	"-p", "F:/Fts/Proj/Test/Interedit/Generated/NiflectGenerated",
				//};
				//argc = sizeof(argvTest) / sizeof(const char*);
				//argv = argvTest;
				
				ParseOptions(argc, argv, info);

				if (gen->InitModuleRegInfo(info))
				{
					CCodeGenData genData;
					gen->Generate(genData);
					gen->Save2(genData);
				}
				else
				{
					ASSERT(false);
				}
			}
		}
	}

	return 0;
}