cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME RoboFightWithTriangleCollision3 )
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
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../Library/include )

link_directories( $ENV{GAME_LIB_DIR}/${SECTION_NAME}/lib )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../Library/lib )

# Project Codes
file(GLOB SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
 )

add_executable(
  ${PROJECT_NAME} WIN32  ${SRCS}
)
add_dependencies( ${PROJECT_NAME} 3DCollisionLibrary )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 18_3DCollision )

target_link_libraries( ${PROJECT_NAME} debug Library_d GameLib_d )
target_link_libraries( ${PROJECT_NAME} optimized Library GameLib )

# File Copy
set( COPY_FILES robo.tga stage.tga stage.txt robo.txt )
include( ../../cmake/copyresource.cmake )
