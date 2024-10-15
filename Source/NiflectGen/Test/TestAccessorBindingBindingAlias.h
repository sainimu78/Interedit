#include "Niflect/NiflectBindingSetting.h"
#include "Engine/BuiltinAccessor.h"
#include "Engine/Test/TestMyTransform.h"
#include "NiflectGen/Test/TestSomeTypes.h"

namespace EngineTypeBindingSettingScope
{
	using namespace Niflect;
	using namespace Engine;


	using MyFloat0 = float;
	using MyFloat1 = MyFloat0;
	using MyFloat2 = float;
	NIF_BS() TBindingSetting<CFloatAccessor, float>;
	NIF_BS() TBindingSetting<CFloatAccessor, MyFloat0>;//fail
	NIF_BS() TBindingSetting<CFloatAccessor, MyFloat1>;//fail
	NIF_BS() TBindingSetting<CFloatAccessor, MyFloat2>;//fail

	using MyString0 = Niflect::CString;
	using MyString1 = MyString0;
	using MyString2 = Niflect::CString;
	
	NIF_BS() TBindingSetting<CFloatAccessor, Niflect::CString>;
	NIF_BS() TBindingSetting<CFloatAccessor, MyString0>;//fail
	NIF_BS() TBindingSetting<CFloatAccessor, MyString1>;//fail
	NIF_BS() TBindingSetting<CFloatAccessor, MyString2>;//fail

	//������ TArrayNif �� std::vector, TStdPairAlias �� std::pair �ķ���Ϊ���ԭʼģ�嶨��Ĳ��������Ƿ���ͬ
	//����ζ���޷����ֲַ��ػ��ı���ģ��, �� TArrayNif �� TArrayNifAlias
	//����Ҫʵ�ִ�����, �ɿ��Ǳ��� aliasChain ÿ��ʱ���ػ���ʽ, ���漰�ǳ������ĵݹ����

	template <typename T0, typename T1>
	NIF_BS() TBindingSetting<CCompoundAccessor, std::pair<T0, T1> >;
	template <typename T0, typename T1>
	NIF_BS() TBindingSetting<CCompoundAccessor, TStdPairAlias<T0, T1> >;//fail
	template <typename T0, typename T1>
	NIF_BS() TBindingSetting<CCompoundAccessor, TestSomeScope::SMyPair<T0, T1> >;
	template <typename T0, typename T1>
	NIF_BS() TBindingSetting<CCompoundAccessor, TestSomeScope::SMyPairAlias<T0, T1> >;//fail

	using MyBits0 = std::vector<bool>;
	using MyBits1 = MyBits0;
	template <typename TInstance, typename T>
	NIF_BS() TBindingSetting<TStlArrayAccessor<TInstance>, std::vector<T> >;
	template <typename TInstance, typename T>
	NIF_BS() TBindingSetting<TStlArrayAccessor<TInstance>, Niflect::TArrayNif<T> >;//ok
	template <typename TInstance>
	NIF_BS() TBindingSetting<TStlBitsArrayAccessor<TInstance>, std::vector<bool> >;//ok
	template <typename TInstance>
	NIF_BS() TBindingSetting<TStlBitsArrayAccessor<TInstance>, MyBits0>;//fail, ע, ��������ӦΪ�� std::vector<T> �ظ�
}