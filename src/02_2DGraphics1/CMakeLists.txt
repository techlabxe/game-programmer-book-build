cmake_minimum_required (VERSION 3.8)
project ( 01_FirstGame )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( drawPixels )
add_subdirectory( NimotsuKunBox )
add_subdirectory( NimotsuKunBoxWithTermination )
add_subdirectory( NimotsuKunTextOnly )
add_subdirectory( NimotuKunDot )
