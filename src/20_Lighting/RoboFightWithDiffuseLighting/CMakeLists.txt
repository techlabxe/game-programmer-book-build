cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME RoboFightWithDiffuseLighting )
set( SECTION_NAME 3DCollision1 )

include( ../../cmake/SampleBase2.cmake )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB LIB_SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/Library/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Library/*.cpp
)

add_executable( 
  ${PROJECT_NAME} WIN32 ${SRCS} ${LIB_SRCS}
)
target_link_libraries( ${PROJECT_NAME} debug GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized GameLib )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 20_Lighting )

# File Copy
set( COPY_FILES robo.tga stage.tga stage.txt robo.txt test.txt)
include( ../../cmake/copyresource.cmake )
