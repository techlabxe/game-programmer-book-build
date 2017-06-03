cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME MultipleWaveFile )
set( SECTION_NAME Sound1 )


set(CMAKE_CONFIGURATION_TYPES "Debug;Release" )
set(CMAKE_SUPPRESS_REGENERATION true)

set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
 )

include( ../../cmake/SampleBase2.cmake )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
 )

add_executable(
  ${PROJECT_NAME} WIN32
  ${SRCS} ${SEQ_SRCS} ${SEQ_GAME_SRCS} ${GAME_SRCS}
)

target_link_libraries( ${PROJECT_NAME} debug GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized GameLib )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 11_Sound1 )

# File Copy
set( COPY_FILES charara.wav dokaan.wav )
include( ../../cmake/copyresource.cmake )
