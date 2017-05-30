cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( AntiAlias )
add_subdirectory( DrawCube )
add_subdirectory( RoboFightUsingFinal )
add_subdirectory( TransparentCubes )
