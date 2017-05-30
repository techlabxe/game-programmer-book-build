cmake_minimum_required (VERSION 3.8)
project ( 05_Sequence1 )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( NimotsuKunWithSequence1 )
add_subdirectory( NimotsuKunWithSequence2 )
