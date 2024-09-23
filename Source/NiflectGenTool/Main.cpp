#include <stdio.h>
#include "NiflectGen/Test/TestParseCpp.h"
#include "NiflectGen/Generator/Generator.h"
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Util/DebugUtil.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Test/TestGen.h"

//����
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
//11. NativeMeta, ͨ��C++ Native������Meta��Ϣ, �����ڶ��巴�������Ϣ, �������Ե���Ϣ

int main()
{
	if (false)
	{
		TestGen::TestCollector();
	}

	if (true)
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
				info.m_moduleName = "Engine";
				info.m_genIncludeBasePath = "NiflectGenerated";
				info.m_genBasePath = "F:/Fts/Proj/Test/Interedit/Generated";
				//begin, ָ����Ҫ������ͷ�ļ��б�
				info.m_vecOriginalHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/EngineObject.h");
				//info.m_vecOriginalHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/Asset.h");
				//end
				//begin, ָ��BindingSettingͷ�ļ��б�
				info.m_vecBindingSettingHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/EngineTypeBindingSetting.h");
				//end
				NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
				gen->SetModuleRegInfo(info);
				gen->Generate();
				gen->Save();
			}
		}
	}

	return 0;
}