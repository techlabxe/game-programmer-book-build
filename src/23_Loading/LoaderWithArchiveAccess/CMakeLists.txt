cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME LoaderWithArchiveAccess )
set( SECTION_NAME Loading )


include( ../../cmake/SampleBase.cmake )


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER 23_Loading )

# File Copy
set( COPY_FILES data.bin )
set( COPY_DIRS data )
include( ../../cmake/copyresource.cmake )

