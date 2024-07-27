FUNCTION(create_source_group relativeSourcePath)
  FOREACH(currentSourceFile ${ARGN})
    FILE(RELATIVE_PATH folder ${relativeSourcePath} ${currentSourceFile})
    GET_FILENAME_COMPONENT(filename ${folder} NAME)
    STRING(REPLACE ${filename} "" folder ${folder})
    IF(NOT folder STREQUAL "")
      STRING(REGEX REPLACE "/+$" "" folderlast ${folder})
      STRING(REPLACE "/" "\\" folderlast ${folderlast})
      SOURCE_GROUP("${folderlast}" FILES ${currentSourceFile})
    ENDIF(NOT folder STREQUAL "")
  ENDFOREACH(currentSourceFile ${ARGN})
ENDFUNCTION(create_source_group)