#include "NiflectGen/CodeWriter/TypeReg/TypeRegCodeWriter.h"
#include "NiflectGen/Util/CursorUtil.h"
#include "NiflectGen/CodeWriter/HardCoded/TypeRegGenHeaderDeclTemplate.h"
#include "NiflectGen/CodeWriter/CodeTemplate.h"
#include "NiflectGen/CodeWriter/CppWriter.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"

namespace NiflectGen
{
	CTypeRegCodeWriter::CTypeRegCodeWriter(const CXCursor& cursor, const STypeRegClassWritingSetting& writingSetting)
		: m_cursor(cursor)
		, m_writingSetting(writingSetting)
	{
		m_typeName = CXStringToCString(clang_getCursorSpelling(m_cursor));
		m_typeNameWithScope = CXStringToCString(clang_getTypeSpelling(clang_getCursorType(m_cursor)));
		Niflect::TArrayNif<Niflect::CString> vecDeclScope;
		FindNamespaceAndScopeNameRecursOld(m_cursor, m_vecNamespace, vecDeclScope);
	}
	void CTypeRegCodeWriter::Init()
	{
	}
	void CTypeRegCodeWriter::WriteGenHeaderDeclaration(const CWritingContext& context, STypeRegGenHeaderDeclarationWritingData& data) const
	{
		data.m_linesTypeDeclaration.push_back(ReplaceLabelToText1(HardCodedTemplate::TypeRegGenHeader_TypeDecl, LABEL_0, m_typeName));

		{
			CCodeTemplate tpl;
			tpl.ReadFromRawData(HardCodedTemplate::TypeRegGenHeader_StaticGetTypeSpecialization);
			CLabelToCodeMapping map;
			MapLabelToText(map, LABEL_1, m_typeNameWithScope);
			tpl.ReplaceLabels(map, data.m_linesStaticGetTypeSpecialization);
		}

		{
			CCodeTemplate tpl;
			tpl.ReadFromRawData(HardCodedTemplate::TypeRegGenHeader_ExposedPrivateMacro);
			CLabelToCodeMapping map;
			MapLabelToText(map, LABEL_0, "reserved_decl_without_namespace");
			MapLabelToText(map, LABEL_2, "reserved_file_id");
			MapLabelToText(map, LABEL_3, "reserved_line_number");
			tpl.ReplaceLabels(map, data.m_linesExposedPrivateMacro);
		}
	}
	void CTypeRegCodeWriter::WritePrivateHeaderIncludePath(const CWritingContext& context, CNoDupPathCollector& includePathRequirement) const
	{
		this->CollectIncludePathFromCursor(context, m_cursor, includePathRequirement);
	}
	void CTypeRegCodeWriter::CollectIncludePathFromCursor(const CWritingContext& context, const CXCursor& cursor, CNoDupPathCollector& includePathRequirement) const
	{
		auto filePath = GetCursorFilePath(cursor);

		if (!NiflectUtil::EndsWith(filePath, NiflectGenDefinition::NiflectFramework::FileName::BypassingHeader))
		{
			includePathRequirement.Add(CIncludesHelper::ConvertToIncludePath(filePath, m_writingSetting.m_vecHeaderSearchPath));
		}
		else
		{
			//todo: ר�Ŵ���STL������:
			//1. ����NiflectSTLDeclaration.h��������, ��ɢ��ͬ����·ͷ�ļ���std::string�Ƶ�string�ļ���
			//2. NiflectSTLDeclaration.h��չ�÷�, ���ڱ�ʾ���ܱ�Binding��STL����
			//3. �Ի�ȡ����·������Ƿ�Ϊ��·����·��, ����ת��ΪSTL����·��
			//4. ���·��ΪNiflectSTLDeclaration.h, �򱨴�, ��ʾ����Binding
			auto type = clang_getCursorType(cursor);
			GenLogError(context.m_log, NiflectUtil::FormatString("Does not support binding type %s", CXStringToCString(clang_getTypeSpelling(type)).c_str()));
		}
	}
}