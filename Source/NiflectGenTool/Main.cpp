#include <stdio.h>
#include "NiflectGen/Test/TestParseCpp.h"
#include "NiflectGen/Generator/Generator.h"
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Util/DebugUtil.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Test/TestGen.h"
#include "NiflectGen/Generator/GenData.h"

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

int main()
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
				info.m_moduleName = "Engine";
				info.m_genIncludeBasePath = "NiflectGenerated";
				info.m_genBasePath = "F:/Fts/Proj/Test/Interedit/Generated";
				//begin, ָ����Ҫ������ͷ�ļ��б�
				info.m_vecModuleHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/EngineObject.h");
				info.m_vecModuleHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/DerivedObject.h");
				info.m_moduleApiMacro = "ENGINE_API";
				info.m_moduleApiMacroHeader = "F:/Fts/Proj/Test/Interedit/Source/Engine/EngineCommon.h";
				info.m_genFileMode = EGeneratingHeaderAndSourceFileMode::ESourceAndHeader;
				//end
				//begin, ָ��BindingSettingͷ�ļ��б�
				info.m_vecAccessorSettingHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/EngineAccessorSetting.h");
				//end
				Test::AddHeaderSearchPaths(info.m_vecParsingHeaderSearchPath, info.m_writingHeaderSearchPaths);
				gen->InitModuleRegInfo(info);
				CCodeGenData genData;
				gen->Generate(genData);
				gen->Save2(genData);
			}
		}
	}

	return 0;
}