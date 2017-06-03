cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( BakudanBitoBackgroundOnly )
add_subdirectory( BakudanBitoSequenceOnly )
add_subdirectory( BakudanBitoWithDynamicObject )
add_subdirectory( NimotsuKunWithSystemFont )
