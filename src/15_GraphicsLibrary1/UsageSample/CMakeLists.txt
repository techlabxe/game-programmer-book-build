cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME UsageSample )
set( SECTION_NAME 3DGraphics1 )

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
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/Library )

link_directories( $ENV{GAME_LIB_DIR}/${SECTION_NAME}/lib )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Library/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Library/*.cpp
 )

add_executable(
  ${PROJECT_NAME} WIN32  ${SRCS}
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 15_GraphicsLibrary )
target_link_libraries( ${PROJECT_NAME} debug GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized GameLib )
