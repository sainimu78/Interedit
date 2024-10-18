#pragma once
#include "NiflectGen/CodeWriter/ModuleReg/ModuleRegCode.h"
#include "NiflectGen/CodeWriter/CodeWriter.h"
#include "NiflectGen/CodeWriter/TypeReg/TypeRegCodeWriter.h"

namespace NiflectGen
{
	struct SSplittedModuleRefInfo
	{
		const CTypeRegWritingData* m_typeRegData;
		const Niflect::CString* m_includePathPrivateH;
	};

	class CSplittedModuleRegWritingContext
	{
	public:
		const Niflect::CString& m_moduleName;
		const Niflect::CString& m_moduleRegBasePath;
		const Niflect::TArrayNif<SSplittedModuleRefInfo>& m_vecTypeRegRefInfo;
	};

	class CSplittedModuleRegGenData
	{
	public:
		CCodeLines m_h;
		CCodeLines m_cpp;
		Niflect::CString m_includePath;
	};

	class CSplittedGenData
	{
	public:
		CCodeLines m_h_or_genH;
		CCodeLines m_cpp_or_privateH;
		Niflect::CString m_h_or_genH_filePath;
		Niflect::CString m_cpp_or_privateH_filePath;
		Niflect::CString m_includePath;
	};

	class CSplittedModuleRegInvokationData
	{
	public:
		CCodeLines m_codeInitTables;
		CCodeLines m_invokationRegisterTypes;
		CCodeLines m_invokationInitTypes;
	};

	class CSplittedModuleRegInfo
	{
	public:
		CSplittedModuleRegInfo()
			: m_taggedTypeHeaderFilePathRef(NULL)
		{
		}
		const Niflect::CString* m_taggedTypeHeaderFilePathRef;
		Niflect::TArrayNif<const CTypeRegWritingData2*> m_vecTypeRegDataRef;
	};

	struct SSplittedFieldLayoutSpecWritingContext
	{
		const CModuleRegInfoValidated& m_moduleRegInfo;
		const Niflect::TArrayNif<CSplittedModuleRegInfo>& m_vecItem;
		CGenLog* m_log;
	};

	class CTypeRegGenData
	{
	public:
		CCodeLines m_genH;
		Niflect::CString m_genHIncludePath;
		CCodeLines m_privateH;
		Niflect::CString m_privateHIncludePath;
	};

	class CFieldLayoutSpecData
	{
	public:
		CCodeLines m_decl;
		Niflect::CString m_declIncludePath;
		CCodeLines m_impl;
		Niflect::CString m_implIncludePath;
	};

	struct SSplittedModuleRegWritingContext2
	{
		const CModuleRegInfoValidated& m_moduleRegInfo;
		const Niflect::TArrayNif<CSplittedModuleRegInfo>& m_vecItem;
		const Niflect::TArrayNif<CFieldLayoutSpecData>& m_vecFieldLayoutSpecData;
		CGenLog* m_log;
	};

	struct SSplittedFieldLayoutSpecWritingData
	{
		Niflect::TArrayNif<CFieldLayoutSpecData>& m_vecFieldLayoutSpecData;
	};

	class CSplittedModuleRegFunctionsName
	{
	public:
		Niflect::CString m_registerTypes;
		Niflect::CString m_initTypes;
	};

	struct SSplittedModuleRegWritingData2
	{
		Niflect::TArrayNif<CSplittedModuleRegGenData>& m_vecSplittedModuleRegGenData;
		Niflect::TArrayNif<CSplittedModuleRegFunctionsName>& m_vecSplittedModuleRegFuncsName;
	};
}