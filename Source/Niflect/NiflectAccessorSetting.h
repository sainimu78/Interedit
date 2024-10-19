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

//�ɱ���Ϊ������ָ�� Accessor ��������
//1. ����� NIF_T ������, �� class, struct
//2. �ṹģ��(Structural Template), �� std::pair
//��������ģ�鷶Χ��ֻ��ʹ��1��
//����ĩβ��1��ʾ TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_Compound_1() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_Compound,_,__LINE__) = 

//enum class ���󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//����ĩβ��1��ʾ TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_EnumClass_1() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_EnumClass,_,__LINE__) = 

//enum ���󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//����ĩβ��1��ʾ TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_EnumBitsMask_1() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_EnumBitsMask,_,__LINE__) = 

//�ɱ���Ϊ������ָ�� Accessor ��������
//1. ����ģ��(Container Template), �� std::vector, std::map
//2. �����ܱ� TCompount, TEnumClass, TEnumBitsMask �󶨵�����
//3. �ػ�ģ��, �� std::vector<bool>
//��������ģ�鷶Χ�ڿɶ��ʹ��, ��ָ����ͬ�����͵� Accessor �򸲸���������ָ���� Accessor
//����ĩβ��2��3��ʾ TSetting ģ����Ҫָ����ģ���������Ϊ2��3
#define NIF_AS_Binding_2_3() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_Binding,_,__LINE__) = 

namespace NiflectAccessorSetting
{
	using TemplaateArgPlaceholder = void*;
	template <typename TAccessorType, typename TBindingType = TemplaateArgPlaceholder, typename TElementBindingType = TemplaateArgPlaceholder>
	class TSetting
	{
	};
}