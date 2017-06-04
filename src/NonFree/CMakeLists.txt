cmake_minimum_required (VERSION 3.8)
project ( SectionAll )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

if( $ENV{GAME_LIB_DIR} STREQUAL "" )
  message( FATAL_ERROR "GAME_LIB_DIR is not defined. You must set GAME_LIB_DIR !" )
endif()

add_subdirectory( BakudanBitoFinal )
add_subdirectory( NimotsuKunFinal )
add_subdirectory( RoboFightFinal )
