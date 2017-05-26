#include "conversion.h"
#include "GameLib/GameLib.h"
#include "GameLib/PseudoXml/Document.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Vector4.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Vector2.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Vector.h"
#include "GameLib/Base/Tank.h"
#include "GameLib/Base/OStringStream.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

using namespace GameLib;
using namespace GameLib::PseudoXml;
using namespace GameLib::Math;
using namespace std;
//こういう別名は歓迎されないが、使用頻度が高すぎるので仕方ない
typedef Element E;
typedef Attribute A;
typedef ConstElement CE;
typedef ConstAttribute CA;
//クラスリスト
class XFile;
class Frame;
class FrameTransformMatrix;
class Mesh;
class MeshNormals;
class MeshMaterialList;

//データ型
//typedef Vector< int > Face;
struct Face{
	int& operator[]( int i ){ return mIndices[ i ]; }
	int mIndices[ 3 ];
	int mFaceId;
};
struct Triangle{
	int& operator[]( int i ){ return mIndices[ i ]; }
	int mIndices[ 3 ];
};
struct IndexPair{
	bool operator<( const IndexPair& a ) const {
		if ( mVertex < a.mVertex ){
			return true;
		}else if ( mVertex > a.mVertex ){
			return false;
		}else if ( mNormal < a.mNormal ){
			return true;
		}else if ( mNormal > a.mNormal ){
			return false;
		}else{
			return ( mPosition < a.mPosition );
		}
	}
	int mPosition;
	int mVertex;
	int mNormal;
};

struct IndexColor{
	int mIndex;
	Vector4 mColor;
};
struct Material{
	Vector4 mFaceColor;
	float mPower;
	Vector3 mSpecularColor;
	Vector3 mEmissiveColor;
	string mTextureFilename;
	string mName;
};
struct Vertex{
	Vertex() : mPosition( 0.f ), mNormal( 0.f ), mUv( 0.f ), mColor( 1.f ){}
	Vector3 mPosition;
	Vector3 mNormal;
	Vector2 mUv;
	Vector4 mColor;
};
struct MaterialIndex{
	bool operator<( const MaterialIndex& a ) const {  //マテリアル番号でソートするための<
		return ( mMaterial < a.mMaterial ); 
	}
	int mMaterial;
	int mFace;
};

//便利関数
void getVectorArray( Array< Vector3 >* dst, CE e );
void getCoord2dArray( Array< Vector2 >* dst, CE e );
void getIndexColorArray( Array< IndexColor >* dst, CE e );
void getMeshFaceArray( Array< Face >* dst, CE e );
void dieIf( bool f );
int getInt( CE e );
float getFloat( CE e );
const char* getString( CE e );
void getName( RefString*, CE e );
void getMaterial( Material*, CE e );
void getMatrix( Matrix34*, CE e );
void getIndexColor( IndexColor*, CE e );

template< class T > void release( Vector< T* >* a ){
	for ( int i = 0; i < a->size(); ++i ){
		SAFE_DELETE( ( *a )[ i ] );
	}
}

//例外クラス
class DataStructureException{
public:
	DataStructureException(){ ASSERT( false ); }
};

//コード本体
class MeshMaterialList{
public:
	//デフォルトのダミーを作ります。
	MeshMaterialList( int vn ){
		mIndices.setSize( vn );
		for ( int i = 0; i < vn; ++i ){
			mIndices[ i ].mMaterial = 0;
			mIndices[ i ].mFace = i;
		}
		mMaterials.setSize( 1 );
		mMaterials[ 0 ].mFaceColor.set( 1.f, 1.f, 1.f, 1.f );
		mMaterials[ 0 ].mPower = 1.f;
		mMaterials[ 0 ].mSpecularColor.set( 1.f, 1.f, 1.f );
		mMaterials[ 0 ].mEmissiveColor.set( 1.f, 1.f, 1.f );
		mGroupSizes.setSize( 1 );
		mGroupSizes[ 0 ] = vn;
	}
	MeshMaterialList( CE e ){
		for ( int i = 0; i < e.childNumber(); ++i ){
			CE c = e.child( i );
			if ( RefString( "Material" ) == c.name() ){
				Material m;
				getMaterial( &m, c );
				mMaterials.push( m );
			}else{
				RefString name;
				getName( &name, c );
				if ( name == "faceIndexes" ){ //頂点取得
					dieIf( mIndices.size() > 0 ); //二個目はない
					mIndices.setSize( c.childNumber() );
					for ( int j = 0; j < c.childNumber(); ++j ){
						CE index = c.child( j );
						mIndices[ j ].mFace = j;
						mIndices[ j ].mMaterial = getInt( index );
					}
				}
			}
		}
		//マテリアルインデクスをマテリアル番号順にソート
		sort( &mIndices[ 0 ], &mIndices[ 0 ] + mIndices.size() );
		//グループの大きさとオフセットを計算して記録
		int matN = mMaterials.size();
		mGroupSizes.setSize( matN );
		for ( int i = 0;i < mGroupSizes.size(); ++i ){
			mGroupSizes[ i ] = 0;
		}
		int prev = mIndices[ 0 ].mMaterial;
		int count = 1;
		for ( int i = 1; i < mIndices.size(); ++i ){
			if ( prev != mIndices[ i ].mMaterial ){
				mGroupSizes[ prev ] = count;
				count = 0;
				prev = mIndices[ i ].mMaterial;
			}
			++count;
		}
		mGroupSizes[ prev ] = count;
	}
	Array< MaterialIndex > mIndices;
	Vector< Material > mMaterials;
	Array< int > mGroupSizes;
};

class MeshNormals{
public:
	MeshNormals( CE e ){
		for ( int i = 0; i < e.childNumber(); ++i ){
			CE c = e.child( i );
			RefString name;
			getName( &name, c );
			if ( name == "normals" ){ //法線取得
				dieIf( mNormals.size() > 0 ); //二個はねえ
				getVectorArray( &mNormals, c );
			}else if ( name == "faceNormals" ){ //三角形取得
				dieIf( mFaces.size() > 0 ); //二個はねえ
				getMeshFaceArray( &mFaces, c );
			}
		}
	}
	Array< Vector3 > mNormals;
	Array< Face > mFaces;
};

class Mesh{
public:
	Mesh( CE e ) :
	mMeshNormals( 0 ),
	mMeshMaterialList( 0 ){
		for ( int i = 0; i < e.attributeNumber(); ++i ){
			CA a = e.attribute( i );
			if ( RefString( "name" ) == a.name() ){
				mName = a.value();
			}
		}
		for ( int i = 0; i < e.childNumber(); ++i ){
			CE c = e.child( i );
			if ( RefString( "MeshNormals" ) == c.name() ){
				dieIf( mMeshNormals != 0 );
				mMeshNormals = NEW MeshNormals( c );
			}else if ( RefString( "MeshTextureCoords" ) == c.name() ){
				dieIf( mUVs.size() > 0 );
				for ( int j = 0; j < c.childNumber(); ++j ){
					CE uvs = c.child( j );
					RefString name;
					getName( &name, uvs );
					if ( name == "textureCoords" ){ //UV取得
						getCoord2dArray( &mUVs, uvs );
					}
				}
			}else if ( RefString( "MeshVertexColors" ) == c.name() ){
				dieIf( mColors.size() > 0 );
				for ( int j = 0; j < c.childNumber(); ++j ){
					CE colors = c.child( j );
					RefString name;
					getName( &name, colors );
					if ( name == "vertexColors" ){ //色取得
						getIndexColorArray( &mColors, colors );
					}
				}
			}else if ( RefString( "MeshMaterialList" ) == c.name() ){
				dieIf( mMeshMaterialList != 0 );
				mMeshMaterialList = NEW MeshMaterialList( c );
			}else if ( RefString( "VertexDuplicationIndices" ) == c.name() ){
				; //無視
			}else if ( RefString( "XSkinMeshHeader" ) == c.name() ){
				; //無視
			}else if ( RefString( "SkinWeights" ) == c.name() ){
				; //無視
			}else{
				RefString name;
				getName( &name, c );
				if ( name == "vertices" ){ //頂点取得
					dieIf( mVertices.size() > 0 ); //二個目はない
					getVectorArray( &mVertices, c );
				}else if ( name == "faces" ){ //三角形取得
					dieIf( mFaces.size() > 0 ); //二個目はない
					getMeshFaceArray( &mFaces, c );
				}else if ( name == "nFaces" ){
					mOldFaceNumber = getInt( c ); //3角形化される前の面数
				}
			}
		}
		if ( !mMeshMaterialList ){ //マテリアルねえ！それは困るので無理矢理作る
			mMeshMaterialList = NEW MeshMaterialList( mVertices.size() );
		}
	}
	~Mesh(){
		SAFE_DELETE( mMeshNormals );
		SAFE_DELETE( mMeshMaterialList );
	}
	int countElement(){
		int matN = mMeshMaterialList->mMaterials.size(); //マテリアル数だけバッチができる
		int textureN = 0;
		for ( int i = 0; i < matN; ++i ){
			if ( mMeshMaterialList->mMaterials[ i ].mTextureFilename.size() > 0 ){
				++textureN; //名前はファイル名から生成する
			}
		}
		return matN + textureN + 1 + 1; //vertexBuffer, IndexBufferで+2
	}
	int toXml( E e, int uniqueId, multimap< Mesh*, string >* batchNameTable, int* elementPos ){
		OStringStream oss; //各種変換用stream
		string meshName;
		if ( mName.size() > 0 ){
			meshName = mName;
		}else{
			oss << "batch" << uniqueId;
			++uniqueId;
			oss.get( &meshName );
			oss.clear();
		}

		//頂点一緒だけど法線違う、みたいなのがあるので、少々面倒な手順を踏んで頂点を複製する。
		//1.インデクス配列を用意
		int newIn = mFaces.size() * 3; //総インデクス数
		Array< IndexPair > indexPairs( newIn );
		//頂点側と法線側のインデクスをたたきこんで回る
		for ( int i = 0; i < mFaces.size(); ++i ){
			for ( int j = 0; j < 3; ++j ){
				int t = i * 3 + j;
				IndexPair& ip = indexPairs[ t ];
				ip.mVertex = mFaces[ i ][ j ];
				ip.mNormal = ( mMeshNormals ) ? mMeshNormals->mFaces[ i ][ j ] : -1;
				ip.mPosition = t;
			}
		}
		//頂点カラーの対応表を先に作ってしまう。
		int oldVn = mVertices.size();
		Array< int > oldVertexToColors( oldVn, -1 );
		int colorN = mColors.size();
		for ( int i = 0; i < colorN; ++i ){
			oldVertexToColors[ mColors[ i ].mIndex ] = i;
		}
		//ソートしちまう
		sort( &indexPairs[ 0 ], &indexPairs[ 0 ] + newIn );
		//新インデクス->新頂点番号テーブル
		Array< int > newIndexToNewVertices( newIn ); 
		Array< Vertex > newVertices( newIn ); //頂点配列は過剰に用意し、後でresizeする
		int prevV = -1;
		int prevN = -1;
		int newVertexPos = -1;
		for ( int i = 0; i < newIn; ++i ){
			const IndexPair& ip = indexPairs[ i ];
			if ( ( prevV != ip.mVertex ) || ( prevN != ip.mNormal ) ){
				++newVertexPos;
				prevV = ip.mVertex;
				prevN = ip.mNormal;
				//頂点格納します。
				Vertex& v = newVertices[ newVertexPos ];
				v.mPosition = mVertices[ ip.mVertex ];
				if ( mMeshNormals ){
					v.mNormal = mMeshNormals->mNormals[ ip.mNormal ];
				}
				if ( mUVs.size() > ip.mVertex ){
					v.mUv = mUVs[ ip.mVertex ]; //位置と添え字は同じ
				}
				int colorIndex = oldVertexToColors[ ip.mVertex ];
				if ( colorIndex != -1 ){
					v.mColor = mColors[ colorIndex ].mColor; //頂点番号から色番号へと変換が必要
				}
			}
			newIndexToNewVertices[ ip.mPosition ] = newVertexPos;
		}
		int newVn = newVertexPos + 1;
		newVertices.setSize( newVn );

		//.x内の面番号から今回三角形化した面番号への変換テーブルを生成する。
		int oldFaceN = mOldFaceNumber;
		int newFaceN = mFaces.size();
		//元の面の数だけ数配列を用意
		Array< int > oldToNewFaceCounts( oldFaceN, 0 ); //元の面の数はこれ
		for ( int i = 0; i < newFaceN; ++i ){
			++oldToNewFaceCounts[ mFaces[ i ].mFaceId ];
		}
		//オフセット変換
		int offset = 0;
		Array< int > oldToNewFaceOffsets( oldFaceN ); //元の面の数はこれ
		for ( int i = 0; i < oldFaceN; ++i ){
			oldToNewFaceOffsets[ i ] = offset;
			offset += oldToNewFaceCounts[ i ];
			oldToNewFaceCounts[ i ] = 0; //位置に使うので初期化
		}
		//配列にぶっこんでまわる
		Array< int > oldToNewFace( newFaceN );
		for ( int i = 0; i < newFaceN; ++i ){
			int t = mFaces[ i ].mFaceId;
			int o = oldToNewFaceOffsets[ t ];
			oldToNewFace[ o + oldToNewFaceCounts[ t ] ] = i;
			++oldToNewFaceCounts[ t ];
		}
		//これで面番号変換テーブルが出来た

		//インデクスバッファを生成する
		int matN = mMeshMaterialList->mMaterials.size(); //マテリアル数だけバッチができる
		Array< Tank< Triangle > > ib( matN ); //拡張したいので中はTank

		offset = 0;
		int ibSizeTotal = 0;
		for ( int i = 0; i < matN; ++i ){
			int n = mMeshMaterialList->mGroupSizes[ i ];
			int start = offset;
			offset += n;
			for ( int j = 0; j < n; ++j ){
				int oldFaceId = mMeshMaterialList->mIndices[ start + j ].mFace;
				int newFaceIdOffset = oldToNewFaceOffsets[ oldFaceId ];
				int newFaceIdCount = oldToNewFaceCounts[ oldFaceId ];
				for ( int k = 0; k < newFaceIdCount; ++k ){
					Triangle tri;
					for ( int l = 0; l < 3; ++l ){
						tri[ l ] = newIndexToNewVertices[ ( k + newFaceIdOffset ) * 3 + l ];
					}
					ib[ i ].add( tri );
					++ibSizeTotal;
				}
			}
		}
		//出せる体勢が整った
		//一意な名前をつける
		int textureN = 0;
		int batchN = 0;
		for ( int i = 0; i < matN; ++i ){
			//バッチ
			if ( ib[ i ].size() > 0 ){ //使ってる三角形がなければ無用
				++batchN;
				//テクスチャ。バッチがないならやはりいらない。
				if ( mMeshMaterialList->mMaterials[ i ].mTextureFilename.size() > 0 ){
					++textureN; //名前はファイル名から生成する
				}
			}
		}
		//頂点バッファ
		Element vbE = e.child( *elementPos );
		++*elementPos;
		vbE.setName( "VertexBuffer" );
		vbE.setChildNumber( newVn );
		vbE.setAttributeNumber( 1 );
		
		string vbName;
		oss << meshName << "_vb";
		oss.get( &vbName );
		oss.clear();
		vbE.attribute( 0 ).set( "name", vbName.c_str() );
		for ( int i = 0; i < newVn; ++i ){
			Element vE = vbE.child( i );
			vE.setName( "Vertex" );
			//全頂点出すものを吟味する。
			const Vertex& v = newVertices[ i ];
			//法線
			bool writeNormal = ( ( v.mNormal.x != 0.f ) || ( v.mNormal.y != 0.f ) || ( v.mNormal.z != 0.f ) );
			bool writeUv = ( ( v.mUv.x != 0.f ) || ( v.mUv.y != 0.f ) );
			bool writeColor = ( ( v.mColor.x != 1.f ) || ( v.mColor.y != 1.f ) || ( v.mColor.z != 1.f ) || ( v.mColor.w != 1.f ) );
			int attrN = 1;
			attrN += ( writeNormal ) ? 1 : 0;
			attrN += ( writeUv ) ? 1 : 0;
			attrN += ( writeColor ) ? 1 : 0;
			vE.setAttributeNumber( attrN );
			int aPos = 0;
			vE.attribute( aPos ).set( "position", &v.mPosition.x, 3 );
			++aPos;
			if ( writeNormal ){
				vE.attribute( aPos ).set( "normal", &v.mNormal.x, 3 );
				++aPos;
			}
			if ( writeUv ){
				vE.attribute( aPos ).set( "uv", &v.mUv.x, 2 );
				++aPos;
			}
			if ( writeColor ){
				vE.attribute( aPos ).set( "color", &v.mColor.x, 4 );
				++aPos;
			}
		}
		//インデクスバッファ
		Element ibE = e.child( *elementPos );
		++*elementPos;
		ibE.setName( "IndexBuffer" );
		ibE.setChildNumber( ibSizeTotal );
		ibE.setAttributeNumber( 1 );
		string ibName;
		oss << meshName << "_ib";
		oss.get( &ibName );
		oss.clear();
		ibE.attribute( 0 ).set( "name", ibName.c_str() );
		int t = 0;
		for ( int i = 0; i < matN; ++i ){
			while ( !ib[ i ].isEnd() ){
				Element iE = ibE.child( t );
				iE.setName( "Triangle" );
				++t;
				iE.setAttributeNumber( 1 );
				iE.attribute( 0 ).set( "indices", ib[ i ].get()->mIndices, 3 );
				ib[ i ].toNext();
			}
		}
		//バッチ行きます
		t = 0;
		for ( int i = 0; i < matN; ++i ){
			if ( ib[ i ].size() == 0 ){ //一個も三角形がないのでこのバッチは出さない。
				continue;
			}
			Element bE = e.child( *elementPos );
			++*elementPos;
			bE.setName( "Batch" );

			const Material& mat = mMeshMaterialList->mMaterials[ i ];

			//バッチ名生成
			string batchName;
			oss << meshName << '_';
			//マテリアルが名前を持っていればそれをくっつける
			if ( mat.mName.size() > 0 ){
				oss << mat.mName;
			}else{ //なければ添え字でも入れとけ
				oss << i;
			}
			oss.get( &batchName );
			oss.clear();
			//後はテクスチャ
			const string& texFilename = mat.mTextureFilename;
			if ( texFilename.size() > 0 ){ //name,vb,ib,ibOffset,triangleNumber,diffuseColor,specularColor,phongExponent,texture
				//テクスチャ行きます
				Element tE = e.child( *elementPos );
				++*elementPos;
				tE.setName( "Texture" );
				tE.setAttributeNumber( 2 );
				//名前はファイル名から拡張子とパスを除いたものをセット
				RefString texName( texFilename.c_str() );
				int periodPos = texName.rfind( '.' );
				int slashPos = texName.rfind( '/' );
				int backSlashPos = texName.rfind( '\\' );
				int last = ( periodPos == -1 ) ? ( texName.size() - 1 ) : ( periodPos - 1 );
				int first = ( slashPos > backSlashPos ) ? ( slashPos + 1 ) : ( backSlashPos + 1 );
				if ( first == -1 ){
					first = 0;
				}
				texName.clamp( first, last );
				tE.attribute( 0 ).set( "name", sizeof( "name" ), texName.get(), texName.size() );
				tE.attribute( 1 ).set( "filename", texFilename.c_str() );
				//バッチのテクスチャ参照名もセット
				bE.setAttributeNumber( 11 );
				bE.attribute( 10 ).set( "texture", sizeof( "texture" ), texName.get(), texName.size() );
			}else{
				bE.setAttributeNumber( 10 );
			}
			batchNameTable->insert( make_pair( this, batchName ) ); //バッチ名前表にぶっこむ
			bE.attribute( 0 ).set( "name", batchName.c_str() );
			bE.attribute( 1 ).set( "vertexBuffer", vbName.c_str() );
			bE.attribute( 2 ).set( "indexBuffer", ibName.c_str() );
			bE.attribute( 3 ).set( "bufferOffset", t * 3 ); //三角形なので×3
			bE.attribute( 4 ).set( "primitiveNumber", ib[ i ].size() );
			bE.attribute( 5 ).set( "diffuseColor", &mat.mFaceColor.x, 3 );
			bE.attribute( 6 ).set( "transparency", mat.mFaceColor.w );
			bE.attribute( 7 ).set( "specularColor", &mat.mSpecularColor.x, 3 );
			bE.attribute( 8 ).set( "specularSharpness", mat.mPower );
			bE.attribute( 9 ).set( "emissionColor", &mat.mEmissiveColor.x, 3 );

			t += ib[ i ].size();
		}
		return uniqueId;
	}
	Array< Vector3 > mVertices;
	Array< Face > mFaces;
	int mOldFaceNumber;
	MeshNormals* mMeshNormals;
	MeshMaterialList* mMeshMaterialList;
	Array< Vector2 > mUVs;
	Array< IndexColor > mColors;
	string mName;
};

class Frame{
public:
	Frame( CE e ){
		mMatrix.setIdentity();
		//名前取得
		for ( int i = 0; i < e.attributeNumber(); ++i ){
			CA a = e.attribute( i );
			if ( RefString( "name" ) == a.name() ){
				mName = a.value();
			}
		}
		//子供について
		for ( int i = 0; i < e.childNumber(); ++i ){
			CE c = e.child( i );
			if ( RefString( "FrameTransformMatrix" ) == c.name() ){
				dieIf( c.childNumber() == 0 );
				getMatrix( &mMatrix, c.child( 0 ) ); //Matrix4x4を渡す
			}else if ( RefString( "Mesh" ) == c.name() ){
				mMeshes.push( NEW Mesh( c ) );
			}else if ( RefString( "Frame" ) == c.name() ){
				mFrames.push( NEW Frame( c ) );
			}
		}
	}
	~Frame(){
		release( &mFrames );
		release( &mMeshes );
	}
	void collectMesh( Vector< Mesh* >* meshes ){
		for ( int i = 0; i < mMeshes.size(); ++i ){
			meshes->push( mMeshes[ i ] );
		}
		for ( int i = 0; i < mFrames.size(); ++i ){
			mFrames[ i ]->collectMesh( meshes );
		}
	}
	int toXml( E e, int nodeId, multimap< Mesh*, string >* batchNameTable ){
		typedef multimap< Mesh*, string > Table;
		typedef Table::iterator It;
		//自分が持つメッシュに属するバッチ名を取ってくる
		vector< string > batchNames;
		for ( int i = 0; i < mMeshes.size(); ++i ){
			pair< It, It > range = batchNameTable->equal_range( mMeshes[ i ] );
			for ( It j = range.first; j != range.second; ++j ){
				batchNames.push_back( j->second );
			}
		}
		//まず自分の情報をセット
		e.setName( "Node" );
		Matrix34 identity;
		identity.setIdentity();
		bool writeMatrix = ( identity != mMatrix );
		int childNumber = ( writeMatrix ) ? 2 : 1;
		int batchN = static_cast< int >( batchNames.size() );
		childNumber += batchN; //バッチ数を足す

		e.setAttributeNumber( childNumber ); //nameと、あればtransform

		int aPos = 0;
		if ( mName.size() > 0 ){
			e.attribute( aPos ).set( "name", mName.c_str() );
			++aPos;
		}else{
			e.attribute( aPos ).set( "name", nodeId );
			++aPos;
			++nodeId;
		}
		if ( writeMatrix ){
			e.attribute( aPos ).set( "transform", &mMatrix.m00, 12 );
			++aPos;
		}
		for ( int i = 0; i < batchN; ++i ){
			e.attribute( aPos ).set( "batch", batchNames[ i ].c_str() );
			++aPos;
		}
		//子の処理
		e.setChildNumber( mFrames.size() ) ;
		for ( int i = 0; i < mFrames.size(); ++i ){
			E c = e.child( i );
			nodeId = mFrames[ i ]->toXml( c, nodeId, batchNameTable );
		}
		return nodeId;
	}
	Matrix34 mMatrix;
	Vector< Mesh* > mMeshes;
	Vector< Frame* > mFrames;
	string mName;
};


class XFile{
public:
	XFile( CE e ){
		for ( int i = 0; i < e.childNumber(); ++i ){
			CE c = e.child( i );
			if ( RefString( "Frame" ) == c.name() ){
				mFrames.push( NEW Frame( c ) );
			}else if ( RefString( "Mesh" ) == c.name() ){
				mMeshes.push( NEW Mesh( c ) );
			}
		}
	}
	Document toXml() const {
		Vector< Mesh* > meshes;
		for ( int i = 0; i < mMeshes.size(); ++i ){
			meshes.push( mMeshes[ i ] );
		}
		for ( int i = 0; i < mFrames.size(); ++i ){
			mFrames[ i ]->collectMesh( &meshes );
		}
		//ドキュメントー
		Document d = Document::create();
		Element root = d.root();
		root.setChildNumber( 1 );
		Element graphicsContainer = root.child( 0 );
		graphicsContainer.setName( "Container" );
		//メッシュポインタ->バッチ名のmultimapを作る
		multimap< Mesh*, string > batchNameTable;
		//エレメント確保
		int elementN = 0;
		for ( int i = 0; i < meshes.size(); ++i ){
			elementN += meshes[ i ]->countElement();
		}
		elementN += mFrames.size();
		graphicsContainer.setChildNumber( elementN );

		//メッシュのXML変換
		int elementPos = 0;
		int id = 0;
		for ( int i = 0; i < meshes.size(); ++i ){
			id = meshes[ i ]->toXml( graphicsContainer, id, &batchNameTable, &elementPos );
		}
		//次。ツリー出します。
		//根元のFrameはTreeにする。
		for ( int i = 0; i < mFrames.size(); ++i ){
			E tree = graphicsContainer.child( elementPos );
			++elementPos;
			tree.setName( "Tree" );
			tree.setAttributeNumber( 1 );
			tree.attribute( 0 ).set( "name", mFrames[ i ]->mName.c_str() );
			tree.setChildNumber( 1 ); //ルートノード
			E rootNode = tree.child( 0 );
			mFrames[ i ]->toXml( rootNode, 0, &batchNameTable );
		}
		
		//TODO:
		return d;
	}
	~XFile(){
		release( &mFrames );
		release( &mMeshes );
	}
	Vector< Frame* > mFrames;
	Vector< Mesh* > mMeshes;
};

//便利関数
void getVectorArray( Array< Vector3 >* dst, CE e ){
	dst->setSize( e.childNumber() );
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE v = e.child( i ); //Vector
		Vector3& newV = ( *dst )[ i ];

		bool xOk, yOk, zOk;
		xOk = yOk = zOk = false;
		for ( int j = 0; j < v.childNumber(); ++j ){
			CE f = v.child( j );
			RefString name;
			getName( &name, f );
			float value = getFloat( f ); 
			if ( name == "x" ){
				newV.x = value;
				xOk = true;
			}else if ( name == "y" ){
				newV.y = value;
				yOk = true;
			}else if ( name == "z" ){
				newV.z = value;
				zOk = true;
			}
		}
		dieIf( !xOk || !yOk || !zOk );
	}
}

void getCoord2dArray( Array< Vector2 >* dst, CE e ){
	dst->setSize( e.childNumber() );
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE v = e.child( i ); //Vector

		Vector2& newV = ( *dst )[ i ];
		bool uOk, vOk;
		uOk = vOk = false;
		for ( int j = 0; j < v.childNumber(); ++j ){
			CE f = v.child( j );
			RefString name;
			getName( &name, f );
			float value = getFloat( f );
			if ( name == "u" ){
				newV.x = value;
				uOk = true;
			}else if ( name == "v" ){
				newV.y = value;
				vOk = true;
			}
		}
		dieIf( !uOk || !vOk );
	}
}

void getMeshFaceArray( Array< Face >* dst, CE e ){
	//一回タンクに入れます
	Tank< Face > tank;
	Face face; //テンポラリ
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE meshFaceE = e.child( i );
		for ( int j = 0; j < meshFaceE.childNumber(); ++j ){
			CE indicesE = meshFaceE.child( j );
			if ( RefString( "Array" ) == indicesE.name() ){ //配列ならこいつだ。
				face.mFaceId = i;
				face.mIndices[ 0 ] = getInt( indicesE.child( 0 ) );
				face.mIndices[ 1 ] = getInt( indicesE.child( 1 ) );
				for ( int k = 2; k < indicesE.childNumber(); ++k ){
					face.mIndices[ 2 ] = getInt( indicesE.child( k ) );
					tank.add( face );
					face.mIndices[ 1 ] = face.mIndices[ 2 ]; //ずらして次へ
				}
			}
		}
	}
	//配列へコピー
	if ( tank.size() > 0 ){
		dst->setSize( tank.size() );
		tank.copyTo( &( *dst )[ 0 ] );
	}
}

void getIndexColorArray( Array< IndexColor >* dst, CE e ){
	dst->setSize( e.childNumber() );
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE t = e.child( i );
		getIndexColor( &( ( *dst )[ i ] ), t );
	}
}

void getColor4( Vector4* dst, CE e ){
	bool rOk, gOk, bOk, aOk;
	rOk = gOk = bOk = aOk = false;
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE c = e.child( i );
		RefString name;
		getName( &name, c );
		float value = getFloat( c );

		if ( name == "red" ){
			dst->x = value;
			rOk = true;
		}else if ( name == "green" ){
			dst->y = value;
			gOk = true;
		}else if ( name == "blue" ){
			dst->z = value;
			bOk = true;
		}else if ( name == "alpha" ){
			dst->w = value;
			aOk = true;
		}
	}
	dieIf( !rOk || !gOk || !bOk || !aOk );
}

void getColor3( Vector3* dst, CE e ){
	bool rOk, gOk, bOk;
	rOk = gOk = bOk = false;
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE c = e.child( i );
		RefString name;
		getName( &name, c );
		float value = getFloat( c );

		if ( name == "red" ){
			dst->x = value;
			rOk = true;
		}else if ( name == "green" ){
			dst->y = value;
			gOk = true;
		}else if ( name == "blue" ){
			dst->z = value;
			bOk = true;
		}
	}
	dieIf( !rOk || !gOk || !bOk );
}


void getIndexColor( IndexColor* dst, CE e ){
	bool indexOk;
	indexOk = false;
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE c = e.child( i );
		if ( RefString( "DWORD" ) == c.name() ){
			dst->mIndex = getInt( c );
			indexOk = true;
		}else{
			getColor4( &dst->mColor, c );
		}
	}
	dieIf( !indexOk );
}

void getMaterial( Material* dst, CE e ){
	//名前取得
	for ( int i = 0; i < e.attributeNumber(); ++i ){
		CA a = e.attribute( i );
		if ( RefString( "name" ) == a.name() ){
			dst->mName = a.value();
		}
	}
	//まだ入れてない記号
	dst->mFaceColor.x = -1e10f;
	dst->mFaceColor.y = -1e10f;
	dst->mFaceColor.z = -1e10f;
	dst->mFaceColor.w = -1e10f;
	dst->mPower = -1e10f;
	dst->mSpecularColor.x = -1e10f;
	dst->mSpecularColor.y = -1e10f;
	dst->mSpecularColor.z = -1e10f;
	dst->mEmissiveColor.x = -1e10f;
	dst->mEmissiveColor.y = -1e10f;
	dst->mEmissiveColor.z = -1e10f;
	for ( int i = 0; i < e.childNumber(); ++i ){
		CE c = e.child( i );
		if ( RefString( "TextureFilename" ) == c.name() ){
			dieIf( c.childNumber() < 1 );
			dst->mTextureFilename = getString( c.child( 0 ) );
		}else{
			RefString name;
			getName( &name, c );
			if ( name == "faceColor" ){
				getColor4( &dst->mFaceColor, c );
			}else if ( name == "power" ){
				dst->mPower = getFloat( c );
			}else if ( name == "specularColor" ){
				getColor3( &dst->mSpecularColor, c );
			}else if ( name == "emissiveColor" ){
				getColor3( &dst->mEmissiveColor, c );
			}
		}
	}
	//不正値対応
	if ( dst->mPower <= 0.f ){ //0以下は不正です。1にして、スペキュラ色を0にします。
		dst->mPower = 1.f;
		dst->mSpecularColor.set( 0.f, 0.f, 0.f );
	}
	dieIf(
		( dst->mFaceColor.x == -1e10f ) ||
		( dst->mFaceColor.y == -1e10f ) ||
		( dst->mFaceColor.z == -1e10f ) ||
		( dst->mFaceColor.w == -1e10f ) ||
		( dst->mPower == -1e10f ) ||
		( dst->mSpecularColor.x == -1e10f ) ||
		( dst->mSpecularColor.y == -1e10f ) ||
		( dst->mSpecularColor.z == -1e10f ) ||
		( dst->mEmissiveColor.x == -1e10f ) ||
		( dst->mEmissiveColor.y == -1e10f ) ||
		( dst->mEmissiveColor.z == -1e10f ) );
}

//例外投げます。assertと違ってtrueなら投げる
void dieIf( bool f ){
	if ( f ){
		throw DataStructureException();
	}
}

//value値をintで取ってくる
int getInt( CE e ){
	int r = 0;
	bool found = false;
	for ( int i = 0; i < e.attributeNumber(); ++i ){
		CA a = e.attribute( i );
		if ( RefString( "value" ) == a.name() ){
			r = a.getIntValue();
			found = true;
		}
	}
	dieIf( !found );
	return r;
}

//value値をfloatで取ってくる
float getFloat( CE e ){
	float r = 0;
	bool found = false;
	for ( int i = 0; i < e.attributeNumber(); ++i ){
		CA a = e.attribute( i );
		if ( RefString( "value" ) == a.name() ){
			r = a.getFloatValue();
			found = true;
		}
	}
	dieIf( !found );
	return r;
}

const char* getString( CE e ){
	const char* r = 0;
	bool found = false;
	for ( int i = 0; i < e.attributeNumber(); ++i ){
		CA a = e.attribute( i );
		if ( RefString( "value" ) == a.name() ){
			r = a.value();
			found = true;
		}
	}
	dieIf( !found );
	return r;
}

//name値を取得
void getName( RefString* dst, CE e ){
	bool found = false;
	for ( int i = 0; i < e.attributeNumber(); ++i ){
		CA a = e.attribute( i );
		if ( RefString( "name" ) == a.name() ){
			*dst = a.value();
			found = true;
		}
	}
	dieIf( !found );
}

void getMatrix( Matrix34* dst, CE e ){
	dieIf( e.childNumber() == 0 ); 
	e = e.child( 0 ); //Arrayが出てくる

	dieIf( e.childNumber() < 16 );
	for ( int i = 0; i < 4; ++i ){ //転置して格納
		for ( int j = 0; j < 3; ++j ){
			CE c = e.child( i * 4 + j ); //j==3は0なのでスキップ
			float t = getFloat( c );
			if ( -t == 0.f ){ //-0を+0にする。
				t = 0.f;
			}
			( *dst )( j, i ) = t; 
		}
	}
	//念のためチェック
	dieIf( getFloat( e.child( 3 ) ) != 0.f );
	dieIf( getFloat( e.child( 7 ) ) != 0.f );
	dieIf( getFloat( e.child( 11 ) ) != 0.f );
	dieIf( getFloat( e.child( 15 ) ) != 1.f );
}

//根元の関数
//#include <fstream>
Document convert( ConstDocument src ){
#if 0
	string s;
	src.convertToString( &s );
	setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
	ofstream out( "converted.txt", ofstream::binary );
	out.write( s.c_str(), s.size() );
	out.close();
#endif
	XFile xFile( src.child( 0 ) ); //XFile
	return xFile.toXml();
}
