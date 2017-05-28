cmake_minimum_required(VERSION 3.4.0)

set( FONT_TGA_FILE ${CMAKE_CURRENT_SOURCE_DIR}/font.tga )
set( FONT_HEADER   ${CMAKE_CURRENT_SOURCE_DIR}/src/fontTextureGenerated.h )

add_custom_command(
  TARGET ${PROJECT_NAME}
  PRE_BUILD
  COMMAND  ${CONVERT_TO_CODE} ${FONT_TGA_FILE} -o ${FONT_HEADER}

  COMMENT "ConvertToCode"
)

