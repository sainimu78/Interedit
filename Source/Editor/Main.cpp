#include <stdio.h>
#include "Engine/Engine.h"
//#include "Windows.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "Niflect/Memory/Default/DefaultMemoryPoolScope.h"
#include "Niflect/Base/NiflectBase.h"
#include "Niflect/Memory/Stats/NiflectMemoryStats.h"
#include "Niflect/Serialization/RwTree.h"
#include "Niflect/Util/DebugUtil.h"
#include "Niflect/Serialization/JsonFormat.h"
#include "Niflect/Serialization/BinaryFormat.h"
#include <fstream>
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Test/TestDefinition.h"
#include "Editor/Diff/DiffArrays.h"
#include "Editor/Diff/Diff2.h"
#include "Niflect/Util/FileStreamUtil.h"

class CMyClassForSimplifiedMakeShared
{
public:
	CMyClassForSimplifiedMakeShared()
		: m_float(1.23f)
		, m_bool(true)
		, m_double(43.21)
	{

	}
	float m_float;
	bool m_bool;
	double m_double;
};

typedef void (*FuncTestAPI_C)();

int main(int argc, char** argv)
{
	TestAPI_Cpp();
	
	{
		using namespace Niflect;
		CDefaultMemoryStatsScope memTestScope;
		auto stats = GetDefaultMemoryStats();
		if (false)//TLS Default Allocator
		{
			CDefaultMemoryPoolScope sdddddd;

			TestMemoryStatsOnThreadsBegin();
			DebugPrintTestTree();
			TestMemoryStatsOnThreadsEnd();
		}
		if (false)//д�����õ� JSON ��ʽ�ļ�
		{
			CRwNode root;
			DebugBuildStructure(&root);
			//std::ofstream ofs(TestDefinition::FilePath::InputJson_JsonFormat, std::ios::binary);
			NiflectUtil::COutputFileStream ofs(TestDefinition::FilePath::InputJson_JsonFormat);
			CJsonFormat::Write(&root, ofs);
		}
		if (false)//JSON ��ʽ��
		{
			{
				CRwNode root;
				//std::ifstream ifs(TestDefinition::FilePath::InputJson_JsonFormat, std::ios::binary);
				NiflectUtil::CInputFileStream ifs(TestDefinition::FilePath::InputJson_JsonFormat);
				CJsonFormat::Read(&root, ifs);

				//std::ofstream ofs(TestDefinition::FilePath::OutputJson_JsonFormat, std::ios::binary);
				NiflectUtil::COutputFileStream ofs(TestDefinition::FilePath::OutputJson_JsonFormat);
				CJsonFormat::Write(&root, ofs);
			}
			ASSERT(NiflectUtil::ReadStringFromFile(TestDefinition::FilePath::InputJson_JsonFormat) == NiflectUtil::ReadStringFromFile(TestDefinition::FilePath::OutputJson_JsonFormat));
		}
		if (false)//JSON ��ʽ�� rapidjson ��д����
		{
			{
				CRwNode root;
				NiflectUtil::CInputFileStream ifs(TestDefinition::FilePath::InputJson_AnimGraphEditorData);
				CJsonFormat::Read(&root, ifs);

				NiflectUtil::COutputFileStream ofs(TestDefinition::FilePath::OutputJson_AnimGraphEditorData);
				CJsonFormat::Write(&root, ofs);
			}

			//д�Ľ������һЩ����
			//1. �޷�ԭ��ת��С��
			//2. ���� rapidjson ��������, �����ַ�Ϊ4���ո�, CJsonFormat Ϊ1�� Tab

			{
				//��Ϊ���Խ�tab����4���ո�
				Niflect::TArrayNif<Niflect::CString> vecLineA;
				Niflect::CString line;
				Niflect::CStringStream ss;
				auto strFromFile = NiflectUtil::ReadStringFromFile(TestDefinition::FilePath::OutputJson_AnimGraphEditorData);
				ss << strFromFile;
				while (std::getline(ss, line))
					vecLineA.push_back(line);
				for (auto& it : vecLineA)
				{
					auto pos = it.find_first_not_of("\t");
					if (pos > 0)
					{
						const auto& cnt = pos;
						for (uint32 idx1 = 0; idx1 < cnt; ++idx1)
							it.erase(it.begin());
						Niflect::CString testIndents;
						for (uint32 idx1 = 0; idx1 < cnt; ++idx1)
							testIndents += "    ";
						it = testIndents + it;
					}
				}
				ss.clear();
				Niflect::TArrayNif<Niflect::CString> vecLineB;
				ss << NiflectUtil::ReadStringFromFile(TestDefinition::FilePath::InputJson_AnimGraphEditorData);
				while (std::getline(ss, line))
					vecLineB.push_back(line);
				ASSERT(vecLineA.size() == vecLineB.size());
				bool same = vecLineA.size() > 0;
				ASSERT(same);
				uint32 diffCnt = 0;
				for (auto idx = 0; idx < vecLineA.size(); ++idx)
				{
					auto& a = vecLineA[idx];
					auto& b = vecLineB[idx];
					if (a != b)
					{
						diffCnt++;
						Niflect::CString strA;
						Niflect::CString strB;
						{
							auto pos = a.find_last_of(' ');
							strA = a.substr(pos + 1, a.length() - pos);
						}
						{
							auto pos = b.find_last_of(' ');
							strB = b.substr(pos + 1, b.length() - pos);
						}
						auto numA = std::stod(strA.c_str());
						auto numB = std::stod(strB.c_str());
						if (numA != numB)
						{
							same = false;
							break;
						}
						else
						{
							ASSERT((strA.length() - strB.length()) == 1);
						}
					}
				}
				ASSERT(diffCnt == 4);
				ASSERT(same);
			}
		}
		if (false)//JSON & �Զ��� Binary ��ʽ�໥ת��
		{
			{
				CRwNode root;
				DebugBuildStructure(&root);
				{
					//std::ofstream ofs(TestDefinition::FilePath::OutputBinary_0, std::ios::binary);
					NiflectUtil::COutputFileStream ofs(TestDefinition::FilePath::OutputBinary_0);
					CBinaryFormat::A_2(&root, ofs);
				}
				{
					//std::ofstream ofs(TestDefinition::FilePath::OutputJson_0, std::ios::binary);
					NiflectUtil::COutputFileStream ofs(TestDefinition::FilePath::OutputJson_0);
					CJsonFormat::Write(&root, ofs);
				}
			}
			{
				CRwNode root;
				//std::ifstream ifs(TestDefinition::FilePath::OutputBinary_0, std::ios::binary);
				NiflectUtil::CInputFileStream ifs(TestDefinition::FilePath::OutputBinary_0);
				CBinaryFormat::B_2(&root, ifs);

				//std::ofstream ofs(TestDefinition::FilePath::OutputJson_1, std::ios::binary);
				NiflectUtil::COutputFileStream ofs(TestDefinition::FilePath::OutputJson_1);
				CJsonFormat::Write(&root, ofs);
			}

			ASSERT(NiflectUtil::ReadStringFromFile(TestDefinition::FilePath::OutputJson_0) == NiflectUtil::ReadStringFromFile(TestDefinition::FilePath::OutputJson_1));
		}
		if (false)
		{
			//TestDiffLCS::SimpleNumberArrays();
			//TestDiffLCS::TestDiff();
			TestDiffLCS::TestLargeData();
			printf("");
		}
		if (true)
		{
			TestDiffEditGraph::TestLargeData();
			printf("");
		}
	}

	//auto hLib = LoadLibrary("Engine.dll");
	//if (hLib != NULL)
	//{
	//	if (auto Func = reinterpret_cast<FuncTestAPI_C>(GetProcAddress(hLib, "TestAPI_C")))
	//		Func();
	//}

	printf("Finished\n");
	fflush(stdout);
	return 0;
}