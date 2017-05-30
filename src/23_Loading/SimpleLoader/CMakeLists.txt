cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME SimpleLoader )
set( SECTION_NAME Loading )


include( ../../cmake/SampleBase.cmake )


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 23_Loading )
