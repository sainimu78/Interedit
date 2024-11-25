cmake_minimum_required(VERSION 3.10.0)

set(ModuleName NiflectGenTool)

set(SourcePath ${RootSourcePath}/NiflectGenTool)

file(GLOB_RECURSE SrcAll ${SourcePath}/*.cpp ${SourcePath}/*.h)

create_source_group(${RootSourcePath} ${SrcAll})

add_executable(${ModuleName} ${SrcAll})

target_link_libraries(${ModuleName} PRIVATE Niflect)
target_link_libraries(${ModuleName} PRIVATE NiflectGen)

target_include_directories(${ModuleName}
	PRIVATE ${RootSourcePath}
)

if(MSVC)
	set_target_properties(${ModuleName} PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG},${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>"
	)
endif()