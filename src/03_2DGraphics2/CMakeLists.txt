cmake_minimum_required (VERSION 3.8)
project ( 03_2DGraphics2 )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( displayImage )
add_subdirectory( displayImageAdditionalBlend )
add_subdirectory( displayImageAlphaBlend )
add_subdirectory( displayImageAlphaTest )
add_subdirectory( displayImagePartially )
add_subdirectory( NimotsuKunAlphaBlend )
add_subdirectory( NimotsuKunAlphaTest )
add_subdirectory( NimotsuKunImage )
