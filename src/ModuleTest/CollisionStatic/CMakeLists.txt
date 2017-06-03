cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME CollisionStatic )
set( SECTION_NAME Final )

include( ../../cmake/SampleBase.cmake )

# File Copy
set( COPY_FILES cube.txt skyNtile.x.txt skySphere.tga tile.tga )
include( ../../cmake/copyresource.cmake )
