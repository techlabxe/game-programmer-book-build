cmake_minimum_required (VERSION 3.8)
project ( Modules )

FIND_PATH(DIRECT3D_INCLUDE_DIR d3dx9.h
 	"$ENV{DXSDK_DIR}/Include"
    	"C:/Program Files/Microsoft DirectX SDK/Include"
    	"C:/DXSDK/Include"
   	NO_DEFAULT_PATH
   	DOC "What is the path where the file d3dx9.h can be found"
  )
  FIND_PATH(DIRECT3D_INCLUDE_DIR d3dx9.h
   	NO_DEFAULT_PATH
   	DOC "What is the path where the file d3dx9.h can be found"
  )
FIND_LIBRARY(DIRECT3D_d3d9_LIBRARY d3d9
	  "$ENV{DXSDK_DIR}/Lib"	
	  "$ENV{DXSDK_DIR}/Lib/x86"
	  "C:/Program Files/Microsoft DirectX SDK/Lib/x86"
          "C:/DXSDK/Include/Lib/x86"
	  NO_DEFAULT_PATH
          DOC "Where can the Direct3D d3d9 library be found"
        )

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set( LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib )
# set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR} )

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR} )
    set( CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


include_directories( ../include)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX _d)
endif()

set( BASE_SRCS 	
	src/Base/DebugStream.cpp
	src/Base/Halt.cpp
	src/Base/IBinaryStream.cpp
	src/Base/MemoryManager.cpp
	src/Base/OStringStream.cpp
	src/Base/PrimeNumber.cpp
	src/Base/RefString.cpp
	src/Base/Compressor.cpp
	src/Base/Conversion.cpp
 )

set( FILE_IO_SRCS
	src/FileIO/OutFile.cpp
	src/FileIO/OutFileImpl.h
	src/FileIO/Archive.h
	src/FileIO/InFile.cpp
	src/FileIO/InFileImpl.h
	src/FileIO/Manager.cpp
	src/FileIO/ManagerImpl.h
)

set( GFX_SRCS
	src/Graphics/Texture.cpp
	src/Graphics/TextureImpl.h
	src/Graphics/VertexBuffer.cpp
	src/Graphics/VertexBufferImpl.h
	src/Graphics/VertexLightingVsGenerated.h
	src/Graphics/IndexBuffer.cpp
	src/Graphics/IndexBufferImpl.h
	src/Graphics/Manager.cpp
	src/Graphics/ManagerImpl.h
	src/Graphics/NoLightingVsGenerated.h
	src/Graphics/PixelLightingPsGenerated.h
	src/Graphics/PixelLightingVsGenerated.h
)
set( MATH_SRCS 
	src/Math/Functions.cpp
	src/Math/Matrix22.cpp
	src/Math/Matrix23.cpp
	src/Math/Matrix33.cpp
	src/Math/Matrix34.cpp
	src/Math/Matrix44.cpp
	src/Math/Random.cpp
	src/Math/vector2.cpp
	src/Math/Vector3.cpp
	src/Math/Vector4.cpp
)

set( INPUT_SRCS
	src/Input/Joystick.cpp
	src/Input/JoystickImpl.h
	src/Input/Keyboard.cpp
	src/Input/Manager.cpp
	src/Input/ManagerImpl.h
	src/Input/Mouse.cpp
)

set( PSEUDOXML_SRCS
	src/PseudoXml/Document.cpp
	src/PseudoXml/DocumentImpl.cpp
	src/PseudoXml/DocumentImpl.h
	src/PseudoXml/Element.cpp
	src/PseudoXml/ElementImpl.h
	src/PseudoXml/Tag.cpp
	src/PseudoXml/Tag.h
	src/PseudoXml/Attribute.cpp
	src/PseudoXml/AttributeImpl.h
)

set( SCENE_SRCS
	src/Scene/Node.h
	src/Scene/NodeTemplate.h
	src/Scene/PrimitiveRenderer.cpp
	src/Scene/StringRenderer.cpp
	src/Scene/Tree.cpp
	src/Scene/TreeImpl.h
	src/Scene/TreeTemplate.cpp
	src/Scene/TreeTemplate.h
	src/Scene/Animation.cpp
	src/Scene/AnimationImpl.h
	src/Scene/AnimationNode.cpp
	src/Scene/AnimationNode.h
	src/Scene/Batch.cpp
	src/Scene/BatchImpl.h
	src/Scene/CollisionDetector.cpp
	src/Scene/CollisionMesh.cpp
	src/Scene/Container.cpp
	src/Scene/ContainerImpl.h
	src/Scene/Curve.h
	src/Scene/Font.cpp
	src/Scene/FontImpl.h
	src/Scene/FontImplBitmap.h
	src/Scene/FontImplOS.h
	src/Scene/Model.cpp
	src/Scene/ModelImpl.h
)

set( SOUND_SRCS
	src/Sound/PlayerImpl.h
	src/Sound/Wave.cpp
	src/Sound/WaveImpl.h
	src/Sound/Manager.cpp
	src/Sound/ManagerImpl.h
	src/Sound/Player.cpp
)

set( THREADING_SRCS
	src/Threading/Thread.cpp
	src/Threading/Event.cpp
	src/Threading/Functions.cpp
	src/Threading/Manager.cpp
	src/Threading/ManagerImpl.h
	src/Threading/Mutex.cpp
	src/Threading/Semaphore.cpp
)

set( WINDOWCREATOR_SRCS
	src/WindowCreator/WindowCreator.cpp
	src/WindowCreator/Resource.h
)

find_path(D3D9_INCLUDE_DIR d3d9.h
    HINTS "$ENV{DXSDK_DIR}/Include"
    )

find_library(D3D9_LIBRARY
	    NAMES d3d9
	    HINTS "$ENV{DXSDK_DIR}/Lib/$ENV{PROCESSOR_ARCHITECTURE}"
)

include_directories( src )
include_directories( ${D3D9_INCLUDE_DIR} )
link_directories(${D3D9_LIBRARY})

set (
  CMAKE_STATIC_LINKER_FLAGS 
  "winmm.lib d3d9.lib dsound.lib dinput8.lib dxguid.lib"
)



# add_custom_target(my_target1 echo @@ command_1 ALL)

add_library( 
	Modules
	STATIC
	${BASE_SRCS}
	${FILE_IO_SRCS}
	${GFX_SRCS}
	${MATH_SRCS}
	${INPUT_SRCS}
	${PSEUDOXML_SRCS}
	${SCENE_SRCS}
	${SOUND_SRCS}
	${THREADING_SRCS}
	${WINDOWCREATOR_SRCS}
)

SET_TARGET_PROPERTIES(
	Modules PROPERTIES
	COMPILE_PDB_NAME_DEBUG   Modules_d
	COMPILE_PDB_NAME_RELEASE Modules

)

add_custom_command(
  TARGET Modules
  PRE_BUILD
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/bin/ConvertToCode.exe ${PROJECT_SOURCE_DIR}/shader/NoLightingVs.obj -o ${PROJECT_SOURCE_DIR}/src/Graphics/NoLightingVsGenerated.h
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/bin/ConvertToCode.exe ${PROJECT_SOURCE_DIR}/shader/VertexLightingVs.obj -o ${PROJECT_SOURCE_DIR}/src/Graphics/VertexLightingVsGenerated.h

  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/bin/ConvertToCode.exe ${PROJECT_SOURCE_DIR}/shader/PixelLightingVs.obj -o ${PROJECT_SOURCE_DIR}/src/Graphics/PixelLightingVsGenerated.h

  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/bin/ConvertToCode.exe ${PROJECT_SOURCE_DIR}/shader/PixelLightingPs.obj -o ${PROJECT_SOURCE_DIR}/src/Graphics/PixelLightingPsGenerated.h

  COMMENT "ConvertToCode"
)

