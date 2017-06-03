cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME NimotsuKun2 )
set( SECTION_NAME FirstGame )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" )
set(CMAKE_SUPPRESS_REGENERATION true)

set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
 )

add_executable( ${PROJECT_NAME} ${SRCS} )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 01_FirstGame )


# File Copy
set( COPY_FILES stageData.txt )
include( ../../cmake/copyresource.cmake )
