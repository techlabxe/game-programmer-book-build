cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME NimotsuKunVariableFrameRate )
set( SECTION_NAME RealTime )


include( ../../cmake/SampleBase.cmake )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 04_RealTime )

# File Copy
set( COPY_FILES nimotsuKunImage2.dds stageData.txt )
include( ../../cmake/copyresource.cmake )

