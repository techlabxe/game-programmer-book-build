cmake_minimum_required(VERSION 3.4.0)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

set(TARGET_FILE_NAME "GameLib" )

# for VCRuntime (STATIC)
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()


if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX _d)
endif()

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  set( 
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} 
    "$ENV{GAME_LIB_DIR}/${PROJECT_FILE_PATH}"
  )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


# Project Includes
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
 )


add_library( 
	${PROJECT_NAME}
	STATIC
	${SRCS}
)


set_target_properties(
  ${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${TARGET_FILE_NAME}
)

add_dependencies(
  ${PROJECT_NAME} Modules
)
target_link_libraries( ${PROJECT_NAME} Modules )

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	COMPILE_PDB_NAME_DEBUG   ${TARGET_FILE_NAME}_d
	COMPILE_PDB_NAME_RELEASE ${TARGET_FILE_NAME}

)

set( CONVERT_TO_CODE ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/bin/ConvertToCode.exe )

