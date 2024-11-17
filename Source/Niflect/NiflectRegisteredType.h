#pragma once
#include "Niflect/NiflectCommon.h"

namespace Niflect
{
	class CNiflectType;

	template <typename T>
	class TRegisteredType
	{
		template <typename T2>
		friend CNiflectType* StaticGetType();
		friend class CNiflectTable;

	public:
		static bool IsValid()
		{
			return s_type != NULL;
		}

	private:
		static CNiflectType* s_type;
	};

	template <typename T>
	CNiflectType* TRegisteredType<T>::s_type = NULL;

	//��δȷ���Ƿ��б�Ҫ֧���ֹ�ע��, ��ȷ����, ����Ĭ��ʵ��, ����������������Ƴ�ʵ��, ��Ϊ���ѵ���ΪTaggedType�̶�����GeneratedBody�е�StaticGetType�ػ�, ��˲�֧���ֹ�ע�����Ҫ����Ĭ�ϵ�ʵ��
	template <typename T>
	CNiflectType* StaticGetType()
	{
		return TRegisteredType<T>::s_type;
	}
}