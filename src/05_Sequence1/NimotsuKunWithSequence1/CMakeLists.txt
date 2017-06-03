cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME NimotsuKunWithSequence1 )
set( SECTION_NAME Sequence1 )


include( ../../cmake/SampleBase.cmake )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 05_Sequence )

# File Copy
set( COPY_FILES nimotsuKunImage2.dds title.dds stageData.txt )
include( ../../cmake/copyresource.cmake )

