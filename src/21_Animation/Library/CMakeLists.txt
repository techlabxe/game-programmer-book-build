cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME AnimationLibrary )
set( SECTION_NAME 3DCollision1 )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

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

# Project Includes
include_directories( ${CMAKE_SOURCE_DIR}/include )
include_directories( $ENV{GAME_LIB_DIR}/${SECTION_NAME}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

link_directories( $ENV{GAME_LIB_DIR}/${SECTION_NAME}/lib )

set( LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${LIB_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_DIR} )
set( CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG ${LIB_DIR} )



# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
 )

add_library( 
	${PROJECT_NAME}
	STATIC
	${SRCS}
)
set_target_properties(
	${PROJECT_NAME} PROPERTIES
	COMPILE_PDB_NAME_DEBUG   Library_d
	COMPILE_PDB_NAME_RELEASE Library

)
set_target_properties(
  ${PROJECT_NAME} PROPERTIES OUTPUT_NAME_DEBUG   Library_d
  ${PROJECT_NAME} PROPERTIES OUTPUT_NAME_RELEASE Library
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 21_Animation )
