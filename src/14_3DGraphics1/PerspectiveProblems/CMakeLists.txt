cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME PerspectiveProblems )
set( SECTION_NAME 3DGraphics1 )

include( ../../cmake/SampleBase.cmake )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 14_3DGraphics )
