#pragma once

#define NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C) A##B##C
#define NIFLECTGENINTERNAL_COMBINE(A,B,C) NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C)

//General Accessor ����, ��� A ������
//�ɱ���Ϊ������ָ�� Accessor ��������
//1. ����ģ��(Container Template 1D), �� std::vector, std::map
//2. �ṹģ��(Structural Template nD, n>=2), �� std::pair
//3. �ػ�ģ��, �� std::vector<bool>
//4. �����ܱ������������󶨵�����. ���, A ������Ҳ�����Ϊһ�ָ�������
//��������ģ�鷶Χ�ڿɶ��ʹ��, ��ָ����ͬ�����͵� Accessor �򸲸���������ָ���� Accessor
//TSetting ģ����Ҫָ����ģ���������Ϊ2��3
#define NIF_AS_A() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_A,_,__LINE__) = 

//Compound Accessor ����, ��� B ������
//class �� struct ���������, �Ҵ���� NIF_T, �ɰ󶨴�����ָ���� Accessor
//��δָ�����͵� A ������ʱ, �����Ͱ󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_B() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_B,_,__LINE__) = 

//Enum Class Accessor ����, ��� C ������
//�� enum class (�� Scoped enum) �����ö������, �ɰ󶨴�����ָ���� Accessor
//��δָ�����͵� A ������ʱ, �����Ͱ󶨴�����ָ���� Accessor
//��������ģ�鷶Χ��ֻ��ʹ��1��
//TSetting ģ����Ҫָ����ģ���������Ϊ1
#define NIF_AS_C() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_AccessorSetting_C,_,__LINE__) = 

//Enum Bits Mask Accessor ����, ��� D ������
//�� enum �����ö������, �ɰ󶨴�����ָ���� Accessor
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