cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME SingleWaveFile )
set( SECTION_NAME Sound1 )


include( ../../cmake/SampleBase2.cmake )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
 )

add_executable(
  ${PROJECT_NAME} WIN32 ${SRCS} )

target_link_libraries( ${PROJECT_NAME} debug GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized GameLib )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 11_Sound1 )

# File Copy
set( COPY_FILES dokaan.wav )
include( ../../cmake/copyresource.cmake )
