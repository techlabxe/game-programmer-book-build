cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( Library )
add_subdirectory( RoboFightWithCuboidCollision )
add_subdirectory( RoboFightWithSphereCollision )
add_subdirectory( RoboFightWithTriangleCollision )
add_subdirectory( RoboFightWithTriangleCollision2 )
add_subdirectory( RoboFightWithTriangleCollision3 )
