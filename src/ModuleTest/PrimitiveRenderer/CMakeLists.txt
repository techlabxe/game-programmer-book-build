cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME PrimitiveRenderer )
set( SECTION_NAME Final )

include( ../../cmake/SampleBase.cmake )

# File Copy
set( COPY_FILES test.tga )
include( ../../cmake/copyresource.cmake )
