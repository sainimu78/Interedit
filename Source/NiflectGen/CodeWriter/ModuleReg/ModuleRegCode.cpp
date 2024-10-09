#include "NiflectGen/CodeWriter/ModuleReg/ModuleRegCode.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"

namespace NiflectGen
{
	CModuleRegInfoValidated::CModuleRegInfoValidated(const CModuleRegInfo& info)
		: m_userProvided(info)
		, m_moduleRegBasePath(NiflectUtil::ConcatPath(NiflectGenDefinition::RelativeDirPath::ModuleReg, info.m_moduleName))
		, m_typeRegBasePath(NiflectGenDefinition::RelativeDirPath::TypeReg)
	{
		//todo: ·����Ч�Լ��, �ظ�·���޳���
	}
}