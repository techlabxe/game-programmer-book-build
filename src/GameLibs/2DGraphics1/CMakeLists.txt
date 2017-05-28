cmake_minimum_required (VERSION 3.8)
# project ( 2DGraphicsFramework )

set( PROJECT_NAME      2DGraphicsFramework )
set( PROJECT_DIR_NAME  2DGraphics1 )
set( PROJECT_FILE_PATH ${PROJECT_DIR_NAME}/lib )

include( ../cmake/GameLib.cmake )
include( ../cmake/MakeFont.cmake )


set( DEST_DIR $ENV{GAME_LIB_DIR}/${PROJECT_DIR_NAME}/include )
set( SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/ )
set( GAMELIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/GameLib )

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${SRC_DIR}/Framework.h ${DEST_DIR}/GameLib

  COMMAND ${CMAKE_COMMAND} -E make_directory ${DEST_DIR}/GameLib/Base
  COMMAND ${CMAKE_COMMAND} -E copy ${GAMELIB_INCLUDE_DIR}/Base/DebugStream.h ${DEST_DIR}/GameLib/Base

  COMMENT "CopyToGameLib"
)

