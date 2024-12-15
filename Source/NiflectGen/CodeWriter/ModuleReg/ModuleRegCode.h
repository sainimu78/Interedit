#pragma once
#include "NiflectGen/CodeWriter/CppWriter.h"

namespace NiflectGen
{
	enum class EGeneratingHeaderAndSourceFileMode
	{
		EHeaderOnly,

		//使用此模式须将生成的文件接入 Build 系统, 以支持在生成后无须执行 Module 的 cmake
		//如未接入 Build 系统, 仅须执行 Module 的 cmake, 将生成的文件加入 Module 即可
		//此模式通常用于方便调试生成的代码, 或利用多处理器编译, 一般情况不使用
		ESourceAndHeader,
	};

	class CModuleRegInfo
	{
	public:
		CModuleRegInfo()
			: m_toGenGeneratedBodyThisType(false)
			, m_toGenStaticModuleReg(false)
			, m_genFileMode(EGeneratingHeaderAndSourceFileMode::EHeaderOnly)
		{
		}
		Niflect::CString m_moduleName;
		Niflect::CString m_genOutputDirPath;
		Niflect::CString m_genSourceOutputDirPath;
		Niflect::TArrayNif<Niflect::CString> m_vecModuleHeader2;
		Niflect::TArrayNif<Niflect::CString> m_vecAccessorSettingHeader;
		Niflect::TArrayNif<Niflect::CString> m_vecModuleHeaderSearchPath2;//header search paths for libclang parsing
		Niflect::TArrayNif<Niflect::CString> m_vecResolverCustomizationHeader;
		Niflect::CString m_moduleApiMacro;
		Niflect::CString m_moduleApiMacroHeader;
		Niflect::CString m_toolHeaderSearchPath;
		bool m_toGenGeneratedBodyThisType;
		bool m_toGenStaticModuleReg;
		EGeneratingHeaderAndSourceFileMode m_genFileMode;
	};

	class CModuleRegInfoValidated
	{
	public:
		bool Init(const CModuleRegInfo& info);
		Niflect::CString GetSourceFileExtForGenFileMode() const;

		Niflect::CString m_moduleGenSource;
		Niflect::CString m_typeRegBasePath;
		Niflect::CString m_genIncludeBasePath;
		Niflect::CString m_moduleRegBasePath;
		Niflect::CString m_moduleRegisteredTypeHeaderFilePath;
		Niflect::CString m_moduleScopeSymbolPrefix;
		Niflect::TArrayNif<Niflect::CString> m_vecParsingHeaderSearchPath;
		CWritingHeaderSearchPaths m_writingHeaderSearchPaths;
		CModuleRegInfo m_userProvided;
	};

	namespace Test
	{
		void InitArgs(CModuleRegInfo& info);
	}
}