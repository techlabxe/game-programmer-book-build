//mviewerが吐いた.xからテンプレート部をコピー。まずこいつらを解釈する。
const char gStandardTemplates[] = "\
template AnimTicksPerSecond {\
 <9e415a43-7ba6-4a73-8743-b73d47e88476>\
 DWORD AnimTicksPerSecond;\
}\
template Frame {\
 <3d82ab46-62da-11cf-ab39-0020af71e433>\
 [...]\
}\
template Matrix4x4 {\
 <f6f23f45-7686-11cf-8f52-0040333594a3>\
 array FLOAT matrix[16];\
}\
template FrameTransformMatrix {\
 <f6f23f41-7686-11cf-8f52-0040333594a3>\
 Matrix4x4 frameMatrix;\
}\
template Vector {\
 <3d82ab5e-62da-11cf-ab39-0020af71e433>\
 FLOAT x;\
 FLOAT y;\
 FLOAT z;\
}\
template MeshFace {\
 <3d82ab5f-62da-11cf-ab39-0020af71e433>\
 DWORD nFaceVertexIndices;\
 array DWORD faceVertexIndices[nFaceVertexIndices];\
}\
template Mesh {\
 <3d82ab44-62da-11cf-ab39-0020af71e433>\
 DWORD nVertices;\
 array Vector vertices[nVertices];\
 DWORD nFaces;\
 array MeshFace faces[nFaces];\
 [...]\
}\
template MeshNormals {\
 <f6f23f43-7686-11cf-8f52-0040333594a3>\
 DWORD nNormals;\
 array Vector normals[nNormals];\
 DWORD nFaceNormals;\
 array MeshFace faceNormals[nFaceNormals];\
}\
template ColorRGBA {\
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>\
 FLOAT red;\
 FLOAT green;\
 FLOAT blue;\
 FLOAT alpha;\
}\
template ColorRGB {\
 <d3e16e81-7835-11cf-8f52-0040333594a3>\
 FLOAT red;\
 FLOAT green;\
 FLOAT blue;\
}\
template Material {\
 <3d82ab4d-62da-11cf-ab39-0020af71e433>\
 ColorRGBA faceColor;\
 FLOAT power;\
 ColorRGB specularColor;\
 ColorRGB emissiveColor;\
 [...]\
}\
template MeshMaterialList {\
 <f6f23f42-7686-11cf-8f52-0040333594a3>\
 DWORD nMaterials;\
 DWORD nFaceIndexes;\
 array DWORD faceIndexes[nFaceIndexes];\
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]\
}\
\
template VertexDuplicationIndices {\
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>\
 DWORD nIndices;\
 DWORD nOriginalVertices;\
 array DWORD indices[nIndices];\
}\
template Coords2d {\
 <f6f23f44-7686-11cf-8f52-0040333594a3>\
 FLOAT u;\
 FLOAT v;\
}\
template MeshTextureCoords {\
 <f6f23f40-7686-11cf-8f52-0040333594a3>\
 DWORD nTextureCoords;\
 array Coords2d textureCoords[nTextureCoords];\
}\
template TextureFilename {\
 <a42790e1-7810-11cf-8f52-0040333594a3>\
 STRING filename;\
}\
template XSkinMeshHeader {\
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>\
 WORD nMaxSkinWeightsPerVertex;\
 WORD nMaxSkinWeightsPerFace;\
 WORD nBones;\
}\
template SkinWeights {\
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>\
 STRING transformNodeName;\
 DWORD nWeights;\
 array DWORD vertexIndices[nWeights];\
 array FLOAT weights[nWeights];\
 Matrix4x4 matrixOffset;\
}\
template Animation {\
 <3d82ab4f-62da-11cf-ab39-0020af71e433>\
 [...]\
}\
template AnimationSet {\
 <3d82ab50-62da-11cf-ab39-0020af71e433>\
 [Animation <3d82ab4f-62da-11cf-ab39-0020af71e433>]\
}\
template FloatKeys {\
 <10dd46a9-775b-11cf-8f52-0040333594a3>\
 DWORD nValues;\
 array FLOAT values[nValues];\
}\
template TimedFloatKeys {\
 <f406b180-7b3b-11cf-8f52-0040333594a3>\
 DWORD time;\
 FloatKeys tfkeys;\
}\
template AnimationKey {\
 <10dd46a8-775b-11cf-8f52-0040333594a3>\
 DWORD keyType;\
 DWORD nKeys;\
 array TimedFloatKeys keys[nKeys];\
}\
";

