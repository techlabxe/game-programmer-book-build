cmake_minimum_required(VERSION 3.4.0)

foreach( file_i ${COPY_FILES} )
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${file_i} ${CMAKE_CURRENT_BINARY_DIR}

  COMMENT "CopyResource"
)
endforeach( file_i )

foreach( dir_i ${COPY_DIRS} )
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${dir_i} ${CMAKE_CURRENT_BINARY_DIR}/${dir_i}

  COMMENT "CopyResource"
)
endforeach( dir_i )

