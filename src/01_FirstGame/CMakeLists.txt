cmake_minimum_required (VERSION 3.8)
project ( 01_FirstGame )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( NimotsuKun )
add_subdirectory( NimotsuKun2 )
add_subdirectory( NimotsuKunBitOperation )
add_subdirectory( NimotsuKunRawMemory )
