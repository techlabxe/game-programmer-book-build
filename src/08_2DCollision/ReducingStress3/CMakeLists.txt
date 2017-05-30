cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME ReducingStress3 )
set( SECTION_NAME 2DActionGame )


include( "../../cmake/SampleBase.cmake" )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 08_Detection )

