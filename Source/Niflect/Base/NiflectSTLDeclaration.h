#pragma once
//���ļ��������ɴ���ʱ��·STLͷ�ļ�, �ɴ�����ٽ�����ʱ, ����˼·������·�������ݶ��ͷ�ļ�
//��STL��·�ļ��޺�׺, ���ᱻCMake����, ��ʹ���к�׺����·�ļ�����Ҫ��CMake�����λ�BypassCode�Ƶ����λ�ñ��ⱻCMake���뵽����
//�Ż��׶οɿ���ʵ��һ��STL�򻯹���, �Զ�������·����
//���ֹ��޸Ĳ���:
//1. �������ִ�����Ϣ, �۲��ҳ����������STLͷ�ļ���, 
//2. ����BypassCode/iflectSTLĿ¼�е������ļ�, ���ļ�����Ϊ�ҵ���STLͷ�ļ���
//3. �ڱ��ļ�����ֲ��Ӧ�Ķ���, ֻ��Ҫ�跨����յ�STL����, ���������ɱ��뼴��, ���嶨����ο�VC��STL
//4. ��ԭ����STLͷ�ļ�������չ��include�����ݲ������ֲ����̫����, Ҳ���ڱ��ļ���ֱ��include, �޸ĺ���Խ�����ʱ, ���޷����ܻ�ϣ���Ż�, �򽫱�Ҫ�Ķ������ֲ�����ļ���
//5. �����ֹ��޸�, �����޸����ݱ������﷨, �ڽ���ʱҲ������Ӧ����ʾ, ��Ҫע��鿴������Ϣ

namespace std
{
template<class _Ty>
	class allocator
	{
	};

template<class _Ty,
	class _Alloc = allocator<_Ty> >
	class vector
	{
	};

template<class _Elem,
	class _Traits,
	class _Alloc>
	class basic_string
	{
	};

template<class _Elem>
	struct char_traits
	{
	};

template<> struct char_traits<char>
	{	// properties of a string or stream char element
	};

typedef basic_string<char, char_traits<char>, allocator<char> >
	string;
	
template<class _Ty = void>
	struct less
	{
	};

template<class _Ty1,
	class _Ty2>
	struct pair
	{
	_Ty1 first;		// the first stored value
	_Ty2 second;	// the second stored value
	};

template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty>,
	class _Alloc = allocator<pair<const _Kty, _Ty> > >
	class map
	{
	};

template<class _Ty = void>
	struct equal_to
	{
	};
	
template<class _Kty>
	struct hash
	{
	};

template<class _Kty,
	class _Ty,
	class _Hasher = hash<_Kty>,
	class _Keyeq = equal_to<_Kty>,
	class _Alloc = allocator<pair<const _Kty, _Ty> > >
	class unordered_map
	{
	};

template<class _Kty,
	class _Pr = less<_Kty>,
	class _Alloc = allocator<_Kty> >
	class set
	{
	};

template <class _Kty, class _Hasher = hash<_Kty>, class _Keyeq = equal_to<_Kty>,
    class _Alloc = allocator<_Kty>>
	class unordered_set
	{
	};
}