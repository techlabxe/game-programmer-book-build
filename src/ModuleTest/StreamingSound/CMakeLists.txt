cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME StreamingSound )
set( SECTION_NAME Final )

include( ../../cmake/SampleBase.cmake )

# File Copy
set( COPY_FILES charara.wav )
include( ../../cmake/copyresource.cmake )
