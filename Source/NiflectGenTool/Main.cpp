#include <stdio.h>
#include "NiflectGen/Test/TestParseCpp.h"
#include "NiflectGen/Generator/Generator.h"
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Util/DebugUtil.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Test/TestGen.h"

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

int main()
{
	if (true)//if (false)
	{
		TestGen::TestCollector();
	}

	if (false)//if (true)
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