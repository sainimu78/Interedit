cmake_minimum_required(VERSION 3.20.0)

set(ModuleName TestModule0)

set(SourcePath ${RootSourcePath}/TestModule0)

file(GLOB_RECURSE SrcAll ${SourcePath}/*.cpp ${SourcePath}/*.h)

create_source_group(${RootSourcePath} ${SrcAll})

add_library(${ModuleName} SHARED ${SrcAll})
set_target_properties(${ModuleName} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
)

target_include_directories(${ModuleName}
	PUBLIC ${RootSourcePath}
)

target_compile_definitions(${ModuleName}
	PRIVATE -DTESTMODULE0_EXPORTS
	PRIVATE -DTESTMODULE0_C_EXPORTS
)

target_link_libraries(${ModuleName} PRIVATE Engine)
target_link_libraries(${ModuleName} PRIVATE Niflect)

set(LinkParentName ${ModuleName})