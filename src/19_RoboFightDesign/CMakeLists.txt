cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( Library )
add_subdirectory( RoboFightIntegrated )
add_subdirectory( RoboFightIntegrated2 )
add_subdirectory( RoboFightMoveOnly )
add_subdirectory( RoboFightWeaponOnly )
add_subdirectory( RoboFightWithFrontend )
