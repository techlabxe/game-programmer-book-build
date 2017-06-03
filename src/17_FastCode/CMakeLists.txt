cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( BinarySearch )
add_subdirectory( CacheMissHell )
add_subdirectory( ListUsingArray )
add_subdirectory( ListUsingNew )
add_subdirectory( TravelingSalesmanProblem )
