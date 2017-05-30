cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(BakudanBitoUsingHardware )
add_subdirectory(DrawColoredAndTexturedTriangle )
add_subdirectory(DrawColoredTriangle )
add_subdirectory(DrawTexturedTriangle )
add_subdirectory(DrawTriangle )
add_subdirectory(Transform )
add_subdirectory(AlphaBlend )
