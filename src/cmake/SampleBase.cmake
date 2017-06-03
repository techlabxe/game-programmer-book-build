cmake_minimum_required(VERSION 3.4.0)

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

link_directories( $ENV{GAME_LIB_DIR}/${SECTION_NAME}/lib )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
 )


if( DEFINED SUBSYSTEM )
else()
set( SUBSYSTEM WIN32 )
endif()

add_executable( 
	${PROJECT_NAME} ${SUBSYSTEM}
	${SRCS}
)

target_link_libraries( ${PROJECT_NAME} debug GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized GameLib )

set( CONVERT_TO_CODE ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/bin/ConvertToCode.exe )

