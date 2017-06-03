cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME RotationAndScaling )
set( SECTION_NAME 2DTransform )

include( ../../cmake/SampleBase.cmake )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 12_2DTransform )

# File Copy
set( COPY_FILES background.dds )
include( ../../cmake/copyresource.cmake )
