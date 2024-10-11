#pragma once
#include "Niflect/NiflectBase.h"

namespace NiflectGen
{
#ifdef EMBEDDING_ELEMENT_BINDING_TYPE_INDEXED_NODE
	class CBindingAccessorIndexedNode;
	using CSharedBindingAccessorIndexedNode = Niflect::TSharedPtr<CBindingAccessorIndexedNode>;
#else
#endif

	class CBindingAccessorIndexedNode
	{
	public:
		CBindingAccessorIndexedNode()
			: m_settingIdx(INDEX_NONE)
			, m_taggedIdx(INDEX_NONE)
			, m_isTemplateFormat(false)
		{
		}
		void InitForAccessorBinding(uint32 foundIdx, const Niflect::CString& headerFilePath);
		void InitForTemplateBegin(const Niflect::CString& signature, uint32 foundIdx);
		void InitForTemplateArguments(const CBindingAccessorIndexedNode& childrenOwner);
		void InitForTemplateEnd();
		void InitForTemplate(const Niflect::CString& signature, uint32 foundIdx, const CBindingAccessorIndexedNode& childrenOwner);
		void InitForClassDecl(const Niflect::CString& signature, uint32 foundIdx, const Niflect::CString& headerFilePath);
		bool IsTemplateFormat() const
		{
			return m_isTemplateFormat;
		}
		bool IsValid() const
		{
			return m_settingIdx != INDEX_NONE || m_taggedIdx != INDEX_NONE;
		}
		bool m_isTemplateFormat;
		uint32 m_settingIdx;
		uint32 m_taggedIdx;//��������Ӧ������һ������ģ��, �ֿ�Ϊ class, struct, enum
		Niflect::TArrayNif<CBindingAccessorIndexedNode> m_vecChild;
		Niflect::CString m_key;
		Niflect::CString m_signature;
#ifdef EMBEDDING_ELEMENT_BINDING_TYPE_INDEXED_NODE
		//Ҫ���� m_vecChild ����, ���޷�֧������ģ�庬��Ա����, ���ṹģ��ɺ���Ա����
		CSharedBindingAccessorIndexedNode m_elem;
#else
#endif
		Niflect::TArrayNif<Niflect::CString> m_vecRequiredHeaderFilePath;
	};

	class CSignatureCode
	{
	public:
		CSignatureCode()
		{
		}
		CSignatureCode(const Niflect::CString& signatureStr, const CBindingAccessorIndexedNode& indexedRoot)
			: m_signatureStr(signatureStr)
			, m_indexedRoot(indexedRoot)
		{
		}
		Niflect::CString m_signatureStr;
		CBindingAccessorIndexedNode m_indexedRoot;
	};

	class CSignatureCodeMapping
	{
	public:
		void SSSSSSSS();
		void DebugGenSignatures(Niflect::TArrayNif<Niflect::CString>& vecSignature);
		Niflect::TArrayNif<CSignatureCode> m_vecItem;
		Niflect::TMap<Niflect::CString, uint32> m_mapSignatureToIndex;
	};
}