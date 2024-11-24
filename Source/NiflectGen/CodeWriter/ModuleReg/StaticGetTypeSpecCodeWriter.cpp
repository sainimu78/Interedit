#include "NiflectGen/CodeWriter/ModuleReg/StaticGetTypeSpecCodeWriter.h"
#include "NiflectGen/CodeWriter/CppWriter.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "NiflectGen/CodeWriter/CodeTemplate.h"
#include "NiflectGen/CodeWriter/HardCoded/StaticGetTypeTemplate.h"

namespace NiflectGen
{
    static Niflect::CString ConvertHeaderFilePathToUnderscoredStyle(const Niflect::CString& filePath)
    {
        Niflect::CString result;
        for (auto& it : filePath)
        {
            if (std::isalnum(it))
                result += it;
            else
                result += '_';
        }
        return result;
    }
    void WriteSplittedStaticGetTypeSpec(const SSplittedCreateTypeAccessorSpecWritingContext& context, CTypeRegStaticGetTypeSpecData& data)
    {
        auto splitsCount = context.m_vecItem.size();
        data.m_vecStaticGetTypeSpecData.resize(splitsCount);
        for (uint32 idx0 = 0; idx0 < splitsCount; ++idx0)
        {
            auto& it0 = context.m_vecItem[idx0];
            if (it0.m_taggedTypeHeaderFilePathAddr != NULL)
            {
                auto headerFilePath = CIncludesHelper::ConvertToIncludePath(*it0.m_taggedTypeHeaderFilePathAddr, context.m_moduleRegInfo.m_writingHeaderSearchPaths.m_vecForRegularConversion);
                auto relativeTypeRegFilePathNoExt = NiflectUtil::RemoveFileExt(headerFilePath);
                auto& staticGetTypeSpecData = data.m_vecStaticGetTypeSpecData[idx0];
                auto typeRegSplittedFilePathNoExt = NiflectUtil::ConcatPath(context.m_moduleRegInfo.m_genIncludeBasePath, relativeTypeRegFilePathNoExt);
                staticGetTypeSpecData.m_genHHeaderFilePath = typeRegSplittedFilePathNoExt + NiflectGenDefinition::FileExt::GenH;
                auto internalRelativeTypeRegFilePathNoExt = NiflectUtil::ConcatPath(context.m_moduleRegInfo.m_typeRegBasePath, ConvertToInternalFilePath(relativeTypeRegFilePathNoExt));
                staticGetTypeSpecData.m_implSourceFilePath = internalRelativeTypeRegFilePathNoExt + NiflectGenDefinition::FileNamePostfix::Gen + context.m_moduleRegInfo.GetSourceFileExtForGenFileMode();

                const bool requiredExportedStaticGetType = !context.m_moduleRegInfo.m_userProvided.m_moduleApiMacro.empty();
                {
                    CCodeLines linesGenHInclude;
                    CHeaderFilePathDataArray vecHeaderData;
                    if (requiredExportedStaticGetType && (!context.m_moduleRegInfo.m_userProvided.m_moduleApiMacroHeader.empty()))
                        vecHeaderData.push_back({ context.m_moduleRegInfo.m_userProvided.m_moduleApiMacroHeader, true });
                    vecHeaderData.push_back({ NiflectGenDefinition::NiflectFramework::FilePath::NiflectTypeHeader, true });
                    CIncludesHelper::ConvertFromHeaderFilePaths(vecHeaderData, context.m_moduleRegInfo.m_writingHeaderSearchPaths, linesGenHInclude);

                    CCodeTemplate tpl1;
                    tpl1.ReadFromRawData(HardCodedTemplate::GenH);
                    CLabelToCodeMapping map;
                    MapLabelToLines(map, LABEL_0, linesGenHInclude);
                    Niflect::CString genHHeaderMacro = ConvertHeaderFilePathToUnderscoredStyle(staticGetTypeSpecData.m_genHHeaderFilePath);
                    {
                        //_gen.h ��Ϊ�� pragma once, �����Ӻ� #error ���, _gen.h ������д��, 
                        // ���ڶ��� CURRENT_FILE_ID, ��˲��ܼ� pragma once, ���� CURRENT_FILE_ID ������ _gen.h ��Ϊ����ʱ����������, ���� CURRENT_FILE_ID Ԥ�������.
                        // ����Ϊ CURRENT_FILE_ID ������Ϊչ���� include ˳�������ȳ��ֵ� FID ����.
                        // �� DerivedObject_gen.h �� GENERATED_BODY ������Ϊ TestModule1_gen.h �е� FID_xxx_GENERATED_BODY
                        MapLabelToText(map, LABEL_11, genHHeaderMacro);
                        MapLabelToText(map, LABEL_12, staticGetTypeSpecData.m_genHHeaderFilePath);
                        MapLabelToText(map, LABEL_13, headerFilePath);
                    }
                    CCodeLines linesTypeDecls;
                    CCodeLines linesStaticGetTypeSpecDecl;
                    //if (requiredExportedStaticGetType)
                    {
                        {
                            for (auto& it1 : it0.m_vecTypeRegDataRef)
                            {
                                for (auto& it2 : it1->m_taggedTypeGeneratedBody.m_linesFullScopedTypeDecl)
                                    linesTypeDecls.push_back(it2);
                            }
                            MapLabelToLines(map, LABEL_1, linesTypeDecls);
                        }
                        {
                            CCodeLines lines;
                            for (auto& it1 : it0.m_vecTypeRegDataRef)
                            {
                                for (auto& it2 : it1->m_taggedTypeGeneratedBody.m_linesStaticGetTypeSpecDecl)
                                    lines.push_back(it2);
                            }
                            ReplaceLabelToNamespaceScopeLines(NiflectGenDefinition::NiflectFramework::ScopeName::Niflect, lines, linesStaticGetTypeSpecDecl);
                            MapLabelToLines(map, LABEL_2, linesStaticGetTypeSpecDecl);
                        }
                    }
                    Niflect::CString genHFileId = NiflectGenDefinition::NiflectFramework::LineNumberMacro::FID + genHHeaderMacro;
                    MapLabelToText(map, LABEL_4, genHFileId);

                    CCodeLines linesMacros;
                    {
                        Niflect::TArrayNif<const CTypeLineNumberMacroData*> vecValidLineNumberMacroData;
                        for (uint32 idx1 = 0; idx1 < it0.m_vecTypeRegDataRef.size(); ++idx1)
                        {
                            auto& lineNumberMacroData = it0.m_vecTypeRegDataRef[idx1]->m_taggedTypeGeneratedBody.m_lineNumberMacroData;
                            auto& lineNumber = lineNumberMacroData.m_generatedBodyLineNumber;
                            if (lineNumber != INDEX_NONE)
                                vecValidLineNumberMacroData.push_back(&lineNumberMacroData);
                        }
                        auto cnt1 = vecValidLineNumberMacroData.size();
                        for (uint32 idx1 = 0; idx1 < cnt1; ++idx1)
                        {
                            auto& lineNumberMacroData = *vecValidLineNumberMacroData[idx1];
                            auto& lineNumber = lineNumberMacroData.m_generatedBodyLineNumber;
                            CCodeLines linesItemsDefinition;
                            CCodeLines linesRootBody;
                            auto cnt2 = lineNumberMacroData.m_vecMacroDefinitionData.size();
                            for (uint32 idx2 = 0; idx2 < cnt2; ++idx2)
                            {
                                auto& it2 = lineNumberMacroData.m_vecMacroDefinitionData[idx2];
                                auto macroName = GenerateLineNumberMacroName(genHFileId, lineNumber, it2.m_namePostfix);
                                WriteLineNumberMacroDefinition(macroName, it2.m_linesBody, linesItemsDefinition);
                                if (idx2 != cnt2 - 1)
                                    macroName += '\\';
                                linesRootBody.push_back(macroName);
                            }
                            CCodeLines linesRootDefinition;
                            {
                                auto macroName = GenerateLineNumberMacroName(genHFileId, lineNumber, NiflectGenDefinition::NiflectFramework::LineNumberMacro::GENERATED_BODY);
                                WriteLineNumberMacroDefinition(macroName, linesRootBody, linesRootDefinition);
                                if (idx1 != cnt1 - 1)
                                    linesRootDefinition.push_back(EscapeChar::EmptyLine);
                            }
                            {
                                CCodeTemplate tpl1;
                                tpl1.ReadFromRawData(HardCodedTemplate::LineNumberMacros);
                                CLabelToCodeMapping map;
                                MapLabelToText(map, LABEL_7, NiflectUtil::FormatString("%u", lineNumber));
                                MapLabelToText(map, LABEL_8, headerFilePath);
                                MapLabelToLines(map, LABEL_9, linesItemsDefinition);
                                MapLabelToLines(map, LABEL_10, linesRootDefinition);
                                Niflect::TSet<Niflect::CString> setReplacedLabel;
                                tpl1.ReplaceLabels(map, linesMacros, &setReplacedLabel);
                            }
                        }
                    }
                    MapLabelToLines(map, LABEL_6, linesMacros);

                    Niflect::TSet<Niflect::CString> setReplacedLabel;
                    tpl1.ReplaceLabels(map, staticGetTypeSpecData.m_genH, &setReplacedLabel);
                }
                {
                    CCodeLines linesGenCppInclude;
                    CHeaderFilePathDataArray vecHeaderData;
                    vecHeaderData.push_back(headerFilePath);
                    CIncludesHelper::ConvertFromHeaderFilePaths(vecHeaderData, context.m_moduleRegInfo.m_writingHeaderSearchPaths, linesGenCppInclude);

                    CCodeTemplate tpl1;
                    tpl1.ReadFromRawData(HardCodedTemplate::GenCpp);
                    CLabelToCodeMapping map;
                    MapLabelToLines(map, LABEL_0, linesGenCppInclude);
                    CCodeLines linesImpl;
                    //if (requiredExportedStaticGetType)
                    {
                        CCodeLines lines;
                        for (auto& it1 : it0.m_vecTypeRegDataRef)
                        {
                            for (auto& it2 : it1->m_taggedTypeGeneratedBody.m_linesStaticGetTypeSpecImpl)
                                lines.push_back(it2);
                        }
                        ReplaceLabelToNamespaceScopeLines(NiflectGenDefinition::NiflectFramework::ScopeName::Niflect, lines, linesImpl);
                    }
                    MapLabelToLines(map, LABEL_3, linesImpl);
                    Niflect::TSet<Niflect::CString> setReplacedLabel;
                    tpl1.ReplaceLabels(map, staticGetTypeSpecData.m_impl, &setReplacedLabel);
                }
            }
        }
    }
}