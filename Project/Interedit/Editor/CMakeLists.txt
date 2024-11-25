cmake_minimum_required(VERSION 3.20.0)

set(ModuleName Editor)

set(SourcePath ${RootSourcePath}/Editor)

file(GLOB_RECURSE SrcAll ${SourcePath}/*.cpp ${SourcePath}/*.h)

create_source_group(${RootSourcePath} ${SrcAll})

add_executable(${ModuleName} ${SrcAll})

target_link_libraries(${ModuleName} PRIVATE Engine)
target_link_libraries(${ModuleName} PRIVATE Niflect)
target_link_libraries(${ModuleName} PRIVATE TestModule0)

target_include_directories(${ModuleName}
	PRIVATE ${RootSourcePath}
)

if(MSVC)
	set_target_properties(${ModuleName} PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG},${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>"
	)
endif()