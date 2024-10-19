#pragma once

//#define NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C) A##B##C
//#define NIFLECTGENINTERNAL_COMBINE(A,B,C) NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C)
//
//#define NIF_AS() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting,_,__LINE__) = 
//
//namespace NiflectAccessorSetting
//{
//	//�ɱ���Ϊ������ָ�� Accessor ��������
//	//1. ����� NIF_T ������, �� class, struct
//	//2. �ṹģ��(Structural Template), �� std::pair
//	//��������ģ�鷶Χ��ֻʹ��1��
//	template <typename TAccessorType>
//	class TCompound
//	{
//	};
//
//	//enum class ���󶨴�����ָ���� Accessor
//	//��������ģ�鷶Χ��ֻʹ��1��
//	template <typename TAccessorType>
//	class TEnumClass
//	{
//	};
//
//	//enum ���󶨴�����ָ���� Accessor
//	//��������ģ�鷶Χ��ֻʹ��1��
//	template <typename TAccessorType>
//	class TEnumBitsMask
//	{
//	};
//
//	using BindingTypePlaceholder = void*;
//
//	//�ɱ���Ϊ������ָ�� Accessor ��������
//	//1. ����ģ��(Container Template), �� std::vector, std::map
//	//2. �����ܱ� TCompount, TEnumClass, TEnumBitsMask �󶨵�����
//	//3. �ػ�ģ��, �� std::vector<bool>
//	//��������ģ�鷶Χ�ڿɶ��ʹ��, ��ָ����ͬ�����͵� Accessor �򸲸���������ָ���� Accessor
//	template <typename TAccessorType, typename TBindingType, typename TElementBindingType = BindingTypePlaceholder>
//	class TBinding
//	{
//	};
//}















#define NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C) A##B##C
#define NIFLECTGENINTERNAL_COMBINE(A,B,C) NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C)

//General Accessor ����, ��� A ������
//�ɱ���Ϊ������ָ�� Accessor ��������
//1. ����ģ��(Container Template 1D), �� std::vector, std::map
//2. �ṹģ��(Structural Template nD, n>=2), �� std::pair
//3. �ػ�ģ��, �� std::vector<bool>
//4. �����ܱ����������������󶨵�����, ���, A ������Ҳ�����Ϊһ�ָ�������
//��������ģ�鷶Χ�ڿɶ��ʹ��, ��ָ����ͬ�����͵� Accessor �򸲸���������ָ���� Accessor
//TSetting ģ����Ҫָ����ģ���������Ϊ2��3
#define NIF_AS_A() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_A,_,__LINE__) = 

//Compound Accessor ����, ��� B ������
//����� NIF_T ������, �� class, struct
//��δָ�����͵� A ������ʱ, �����Ͱ󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_B() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_B,_,__LINE__) = 

//Enum Class Accessor ����, ��� C ������
//enum class ���󶨴�����ָ���� Accessor
//��δָ�����͵� A ������ʱ, �����Ͱ󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_C() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_C,_,__LINE__) = 

//Enum Bits Mask Accessor ����, ��� D ������
//enum ���󶨴�����ָ���� Accessor
//��δָ�����͵� A ������ʱ, �����Ͱ󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_D() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_D,_,__LINE__) = 

namespace NiflectAccessorSetting
{
	using TemplaateArgPlaceholder = void*;
	template <typename TAccessorType, typename TBindingType = TemplaateArgPlaceholder, typename TElementBindingType = TemplaateArgPlaceholder>
	class TSetting
	{
	};
}