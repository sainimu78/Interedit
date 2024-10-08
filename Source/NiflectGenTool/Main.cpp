#include <stdio.h>
#include "NiflectGen/Test/TestParseCpp.h"
#include "NiflectGen/Generator/Generator.h"
#include "Niflect/Util/StringUtil.h"
#include "Niflect/Util/DebugUtil.h"
#include "NiflectGen/Base/NiflectGenDefinition.h"
#include "Niflect/Memory/Default/DefaultMemory.h"
#include "NiflectGen/Test/TestGen.h"

//概念
//1. StaticRegStage, 在静态初始化阶段的注册过程
//2. InitialRegStage, 在逻辑初始化(或App等流程上的初始化)阶段的注册过程
//3. TypeReg, 被标记类型对应的反射生成代码的注册信息, 由一种class所表示, 这种class也称作TypeRegClass
//4. UntaggedReg, Builtin类型, 模板类型的反射生成代码, 只作用于模块内部
//5. ModuleReg, 包含模块所有反射生成代码的注册信息
//6. SplittedModuleReg, 为利用多线程编译, ModuleReg可能被分成多个注册信息, 合并定义注册到原ModuleReg中
//7. Accessor, 定义Field的序列化方式, 或称读写方式
//8. FieldLayout, 由Accessor构成的树型结构, 表示Native对象所有被标记成员变量(Field)的读写方式
//9. AccessorBinding, Field类型对应的Accessor类, 用于生成代码时指定对应的Accessor
//10. NiflectType, 表示运行时的类的信息, 或称作反射信息类, 可通过该信息构造对应的Native对象
//11. NativeMeta, 通过C++ Native风格定义的Meta信息, 如用于定义类的反射信息, 属性的反射信息
//12. ContainerTemplate1D, 1维容器模板, 代码生成逻辑上的模板参数只有1个, 如std::vector, std::shared_ptr, 对于std::map, 在代码生成逻辑上std::pair作为其模板参数, 而非first与second作为模板参数, 因此std::map在代码生成逻辑上也属于1维容器模板
//13. StructuralTemplateND, N维结构模板, 其中N>=2, 每个模板参数对应一个Field, 如std::pair
//14. TAccesorBinding, Accessor与解析类型的绑定配置, 对于模板的绑定, 1维BindingType认定为ContainerTemplate1D, 大于1维的BindingType其最后一个BindingType认定为StructuralTemplateND, 其余认定为ContainerTemplate1D

int main()
{
	if (true)//if (false)
	{
		TestGen::TestCollector();
	}

	if (false)//if (true)
	{
		using namespace NiflectGen;

		using namespace Niflect;
		//auto table = CreateTypeBindingTable();
		auto memTest = GetDefaultMemoryStats();
		{
			auto gen = CreateGenerator();
			if (true)
			{
				CModuleRegInfo info;
				info.m_moduleName = "Engine";
				info.m_genIncludeBasePath = "NiflectGenerated";
				info.m_genBasePath = "F:/Fts/Proj/Test/Interedit/Generated";
				//begin, 指定需要解析的头文件列表
				info.m_vecOriginalHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/EngineObject.h");
				//info.m_vecOriginalHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/Asset.h");
				//end
				//begin, 指定BindingSetting头文件列表
				info.m_vecBindingSettingHeader.push_back("F:/Fts/Proj/Test/Interedit/Source/Engine/EngineTypeBindingSetting.h");
				//end
				NiflectGenDefinition::Test::AddBasicHeaderSearchPaths(info.m_vecHeaderSearchPath);
				gen->SetModuleRegInfo(info);
				gen->Generate();
				gen->Save();
			}
		}
	}

	return 0;
}