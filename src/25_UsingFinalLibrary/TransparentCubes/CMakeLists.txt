cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME TransparentCubes )
set( SECTION_NAME Final )

include( ../../cmake/SampleBase.cmake )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 25_UsingFinalLibrary )
