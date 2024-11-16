#pragma once

namespace Niflect
{
	//Native Meta
	class CNata;
	using CSharedNata = TSharedPtr<CNata>;
	class CNata
	{
		template <typename TNata>
		friend CSharedNata MakeDerivedNata(const TNata& derived);

	private:
		virtual bool _DebugFuncForDynamicCast() const { return true; }//��Ϊ��̬������ͱ������, ���Ѷ���ǵ����õ�virtual��������Ƴ�, ��ע: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 
	};

	template <typename TNata>
	static CSharedNata MakeDerivedNata(const TNata& derived)
	{
		ASSERT(derived._DebugFuncForDynamicCast());//����Ƿ��� CNata Ϊ����
		return Niflect::MakeShared<TNata>(derived);
	}
}