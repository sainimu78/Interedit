#include "Engine/Engine.h"
#include <stdio.h>

void TestAPI_Cpp()
{
	printf("Invoke C++ API\n");
	fflush(stdout);
}
void TestAPI_C()
{
	printf("Invoke C API\n");
	fflush(stdout);
}

#ifdef ENABLED_TEST_MODULEREG_GEN_BY_NGT
#include "ModuleReg/Engine/Engine.private.h"
#include "BuiltinAccessor.h"
#include "Engine/EngineObject.h"
#include "Engine/Asset.h"
#include "Niflect/Serialization/RwTree.h"
#include "Niflect/Serialization/JsonFormat.h"

////begin, ����֤�޷�ͨ���ػ�����ԭʼ���������(using��typedef����ı���)
////using Nihao = float;
////typedef float SSSSS;
//namespace Niflect
//{
//	template <>
//	CNiflectType* StaticGetType<Nihao>()
//	{
//		return TInternalRegisteredType<Nihao>::s_type;
//	}
//	template <>
//	bool __InternalStaticIsValid<Nihao>()
//	{
//		return TInternalRegisteredType<Nihao>::s_type != NULL;
//	}
//	template <>
//	CNiflectType* StaticGetType<SSSSS>()
//	{
//		return TInternalRegisteredType<SSSSS>::s_type;
//	}
//	template <>
//	bool __InternalStaticIsValid<SSSSS>()
//	{
//		return TInternalRegisteredType<SSSSS>::s_type != NULL;
//	}
//}
////end

void TestEngineCreate()
{
	{
		//ģ��� Static Reg
		auto reg = Niflect::CNiflectRegistration::GetMutable();
		auto table = reg->GetMutableMiscTable();
		//��δʵ��֧��ֱ�������ޱ�ǩBindingType��TypeReg, �����ģ��� Static Reg �׶���ע��
		//Ϊ�ޱ�ǩ��BindingType�취Ϊ: ����BindingType, ��ΪBuiltIn����, �����ɰ�Ĭ��FieldLayout����������TypeReg
		//��һ��˼·��֧�ֱ����ӱ�ǩ, ��Ҫ����BindingSetting�е�BindingTypeҲʹ�ñ���, ������������ʹ��, ��Ϊ�Ա����ӱ�ǩ������ϣ��ר��Ϊ���ӱ�ǩ�ı���ע��һ������
		table->RegisterType<Niflect::CNiflectType, float>("float", &Niflect::__InternalCreateFieldLayoutForFunctionPointer<Engine::CFloatAccessor>);
		table->RegisterType<Niflect::CNiflectType, bool>("bool", &Niflect::__InternalCreateFieldLayoutForFunctionPointer<Engine::CBoolAccessor>);

		////begin, ����֤�޷�ͨ���ػ�����ԭʼ���������(using��typedef����ı���)
		//table->RegisterType<Niflect::CNiflectType, Nihao>("Nihao", &Niflect::__InternalCreateFieldLayoutForFunctionPointer<Engine::CFloatAccessor>);
		//table->RegisterType<Niflect::CNiflectType, SSSSS>("SSSSS", &Niflect::__InternalCreateFieldLayoutForFunctionPointer<Engine::CFloatAccessor>);
		////end
	}
	{
		//Initial Reg
		GeneratedNiflectRegistrationInitialReg();
		GeneratedNiflectRegistrationInitTypes();
	}
}
void TestEngineDestroy()
{
	GeneratedNiflectRegistrationRelease();
}
void TestEngineRun()
{
	Niflect::TArrayNif<Niflect::CNiflectType*> vec;
	vec.push_back(Niflect::StaticGetType<float>());
	vec.push_back(Niflect::StaticGetType<bool>());
	vec.push_back(Niflect::StaticGetType<Engine::CEngineObject>());
	vec.push_back(Niflect::StaticGetType<Engine::CAsset>());
	for (auto& it : vec)
		printf("%s, ", it->GetTypeName().c_str());
	printf("\n");

	{
		for (auto& type : vec)
		{
			using namespace RwTree;
			using namespace Niflect;
			auto sharedSrcData = type->MakeSharedInstance<void*>();
			auto layout = type->CreateFieldLayout();
			if (type == Niflect::StaticGetType<float>())
			{
				auto& srcData = *reinterpret_cast<float*>(sharedSrcData.Get());
				srcData = 12.3f;
			}
			else if (type == Niflect::StaticGetType<bool>())
			{
				auto& srcData = *reinterpret_cast<bool*>(sharedSrcData.Get());
				srcData = true;
			}
			else if (type == Niflect::StaticGetType<Engine::CEngineObject>())
			{
				auto& srcData = *reinterpret_cast<Engine::CEngineObject*>(sharedSrcData.Get());
				srcData.InitForTest();
			}
			else if (type == Niflect::StaticGetType<Engine::CAsset>())
			{
				auto& srcData = *reinterpret_cast<Engine::CAsset*>(sharedSrcData.Get());
				srcData.InitForTest();
			}
			else
			{
				ASSERT(false);
			}

			CRwNode rw;
			layout->SaveToRwNode(sharedSrcData.Get(), &rw);
			CStringStream ss;
			CJsonFormat::Write(&rw, ss);

			auto sharedDstData = type->MakeSharedInstance<void*>();
			layout->LoadFromRwNode(sharedDstData.Get(), &rw);

			if (type == Niflect::StaticGetType<float>())
			{
				auto& srcData = *reinterpret_cast<float*>(sharedSrcData.Get());
				auto& dstData = *reinterpret_cast<float*>(sharedDstData.Get());
				ASSERT(srcData == dstData);
			}
			else if (type == Niflect::StaticGetType<bool>())
			{
				auto& srcData = *reinterpret_cast<bool*>(sharedSrcData.Get());
				auto& dstData = *reinterpret_cast<bool*>(sharedDstData.Get());
				ASSERT(srcData == dstData);
			}
			else if (type == Niflect::StaticGetType<Engine::CEngineObject>())
			{
				auto& srcData = *reinterpret_cast<Engine::CEngineObject*>(sharedSrcData.Get());
				auto& dstData = *reinterpret_cast<Engine::CEngineObject*>(sharedDstData.Get());
				ASSERT(srcData == dstData);
			}
			else if (type == Niflect::StaticGetType<Engine::CAsset>())
			{
				auto& srcData = *reinterpret_cast<Engine::CAsset*>(sharedSrcData.Get());
				auto& dstData = *reinterpret_cast<Engine::CAsset*>(sharedDstData.Get());
				ASSERT(srcData == dstData);
			}
		}
	}

	////begin, ����֤�޷�ͨ���ػ�����ԭʼ���������(using��typedef����ı���)
	//auto d = Niflect::StaticGetType<Nihao>();
	//auto e = Niflect::StaticGetType<SSSSS>();
	//printf("");
	////end

}

//todo: 2024.08.11, �ƻ���Ϊ
//�ֹ�ע��ľ�̬�������� Static Reg
//���ɵ�ע�������� Initial Reg
//�����׶�ע����� Dynamic Reg
//
//�� Initial Reg ֮����ܽ��� CreateFieldLayout
//����ζ��֮ǰ�׶ε�ע����Ϣ���κ�����, ֻ��Ҫע�����ͱ�����

//using namespace NiflectModuleReg;
//using namespace Niflect;

//CNiflectRegistration* TestCreateModuleReg0()
//{
//	//CreateModuleRegistration();
//	auto reg = CNiflectRegistration::Get();
//	reg->InitTables();
//	reg->RegisterTypes();
//	reg->InitTypes();
//	reg->InitMethods();
//	reg->InitTypesAccessorTree();
//	return reg;
//}
//void TestDestroyModuleReg0()
//{
//	auto reg = CNiflectRegistration::Get();
//	reg->ClearTables();
//	CNiflectRegistration::StaticRelease();
//	//DestroyModuleRegistration();
//}
#else
#include "Niflect/NiflectRegistration.h"
Niflect::CNiflectRegistration* GeneratedNiflectRegistrationGet()
{
	return NULL;
}
void GeneratedNiflectRegistrationInitialReg()
{
}
void GeneratedNiflectRegistrationInitTypes()
{
}
#endif