#pragma once
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Util/ConstCharConcat.h"

#define DEVELOPMENT_ROOT_SOURCE_PATH "E:/S3/S3ResourceEditor_another/engine_ref"
#define ROOT_TEST_PATH "F:/Fts/Proj/Test/Interedit/Source/NiflectGen/Test"

namespace NiflectGenDefinition
{
	namespace FileExt
	{
		constexpr const char* Cpp = ".cpp";
		constexpr const char* H = ".h";
		constexpr const char* PrivateH = "_private.h";
		constexpr const char* GenH = "_gen.h";

		static bool IsCpp(const Niflect::CString& text)
		{
			return NiflectUtil::EndsWith(text, Cpp);
		}
		static bool IsH(const Niflect::CString& text)
		{
			return NiflectUtil::EndsWith(text, H);
		}
	}

	namespace RelativeDirPath
	{
		constexpr const char* ModuleReg = "ModuleReg";
		constexpr const char* TypeReg = "TypeReg";
	}

	namespace CodeTag
	{
		constexpr const char* BindingSetting = "__NiflectGen_BindingSetting";
		constexpr const char* Type = "__NiflectGen_Type";
		constexpr const char* Field = "__NiflectGen_Field";
		constexpr const char* Method = "__NiflectGen_Method";
		constexpr const char* EnumConstant = "__NiflectGen_EnumConstant";

		static bool IsBindingSetting(const Niflect::CString& text)
		{
			return NiflectUtil::StartsWith(text, NiflectGenDefinition::CodeTag::BindingSetting);
		}
		static bool IsType(const Niflect::CString& text)
		{
			return NiflectUtil::StartsWith(text, NiflectGenDefinition::CodeTag::Type);
		}
		static bool IsField(const Niflect::CString& text)
		{
			return NiflectUtil::StartsWith(text, NiflectGenDefinition::CodeTag::Field);
		}
		static bool IsMethod(const Niflect::CString& text)
		{
			return NiflectUtil::StartsWith(text, NiflectGenDefinition::CodeTag::Method);
		}
	}

	namespace CodeStyle
	{
		constexpr const bool AddingSpaceBetweenNestedTemplateRightAngleBrackets = true;
		static void TryFormatNestedTemplate(Niflect::CString& str)
		{
			if (AddingSpaceBetweenNestedTemplateRightAngleBrackets)
			{
				if (str.back() == '>')
					str += ' ';
			}
		}
		static void TemplateAngleBracketL(Niflect::CString& str)
		{
			str += '<';
		}
		static void TemplateAngleBracketR(Niflect::CString& str)
		{
			TryFormatNestedTemplate(str);
			str += '>';
		}
	}

	namespace Path
	{
		constexpr const char* CLangParserArgs_I2[] = {
			CONCAT_CONST_CHAR_2(DEVELOPMENT_ROOT_SOURCE_PATH, "/S3Engine"),
			//todo: �ִ���δ�ϲ�������, �����ʹ��SampleTest��·��, �ϲ�����Ƴ�
			CONCAT_CONST_CHAR_2(DEVELOPMENT_ROOT_SOURCE_PATH, "/SampleTest/SampleGamePrototyping/Cos/Client/ReflectionSystemDemo"),
			//deprecated: ����ͷ�ļ�Ҫ����������.gen.h, ����޷���������.h, ����Ҫ���ɸ�.h
			CONCAT_CONST_CHAR_2(DEVELOPMENT_ROOT_SOURCE_PATH, "/Generated"),
		};

		//������·clang_parseTranslationUnit������STLͷ�ļ�����, �ɴ�����ٺ�ʱ
		//todo: ��·���뼫��, ��ֻ���ڴ�������
		//constexpr const char* BypassingSTL = CONCAT_CONST_CHAR_2(DEVELOPMENT_ROOT_SOURCE_PATH, "/SampleTest/SampleGamePrototyping/Cos/Client/ReflectionSystemDemo/Niflect/Base/BypassCode/NiflectSTL");
	}

	namespace NiflectFramework
	{
		namespace AccessorTypeName
		{
			constexpr const char* Field = "CAccessor";
			constexpr const char* ArrayField = "CArrayField";
			constexpr const char* CompoundField = "CCompoundAccessor";
		}

		namespace InfoTypeName
		{
			constexpr const char* NiflectType = "CNiflectType";
			constexpr const char* Struct = "CStruct";
			constexpr const char* Class = "CClass";
			constexpr const char* Enum = "CEnum";
		}

		namespace FileName
		{
			constexpr const char* BypassingHeader = "NiflectSTLDeclaration.h";
		}

		namespace FilePath
		{
#ifdef USER_PROVIDED_CCOMPONENTFIELD_CLASS_DEFINITION
			constexpr const char* CompoundFieldHeader = "Niflect/Field/CompoundField.h";
#endif
		}

		namespace Setting
		{
			constexpr uint32 ThresholdLinesCountForModuleRegSplitting = 10;
		}
	}
	namespace Test
	{
		static void AddBasicHeaderSearchPaths(Niflect::TArrayNif<Niflect::CString>& vec)
		{
			//begin, ������ͷ�ļ�Դ������Ŀ¼
			vec.push_back("F:/Fts/Proj/Test/Interedit/Source");
			//end
			//begin, ������·clang_parseTranslationUnit������STLͷ�ļ�����, �ɴ�����ٺ�ʱ
			//todo: ��·���뼫��, ��ֻ���ڴ�������
			vec.push_back("F:/Fts/Proj/Test/Interedit/Source/BypassCode/NiflectSTL");
			//end
		}
	}
}