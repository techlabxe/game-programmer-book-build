cmake_minimum_required (VERSION 3.8)
project ( 04_Realtime )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( NimotsuKunConstantFrameRate )
add_subdirectory( NimotsuKunRealTime )
add_subdirectory( NimotsuKunRealTime2 )
add_subdirectory( NimotsuKunSmoothMove )
add_subdirectory( NimotsuKunVariableFrameRate )
add_subdirectory( Tearing )
