#include "Engine/Engine.h"
#include <stdio.h>
#include "Engine/Engine.private.h"

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