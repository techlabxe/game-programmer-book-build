cmake_minimum_required (VERSION 3.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory( ChangeFrequency )
add_subdirectory( ChangeTimbre )
add_subdirectory( ChangeVolume )
add_subdirectory( MultiTrack )
add_subdirectory( Scale )
add_subdirectory( SimpleWave )
add_subdirectory( SingleTrack )
add_subdirectory( Superposition )
add_subdirectory( WavFile )
