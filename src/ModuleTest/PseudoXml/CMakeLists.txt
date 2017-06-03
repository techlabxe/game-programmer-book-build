cmake_minimum_required (VERSION 3.8)

set( PROJECT_NAME PseudoXml )
set( SECTION_NAME Final )

include( ../../cmake/SampleBase.cmake )


# File Copy
set( COPY_FILES data.txt data2.txt  )
include( ../../cmake/copyresource.cmake )
