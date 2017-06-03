cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME MinimumImplementation )
set( SECTION_NAME 3DGraphics1 )

include( ../../cmake/SampleBase2.cmake )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Library/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Library/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PseudoXml/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/PseudoXml/*.cpp
 )

add_executable( 
  ${PROJECT_NAME} WIN32 ${SRCS} 
)
target_link_libraries( ${PROJECT_NAME} debug GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized GameLib )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 16_PseudoXml )

# File Copy
set( COPY_FILES grid.tga data.txt camera.txt )
include( ../../cmake/copyresource.cmake )
