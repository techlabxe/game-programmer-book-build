#include "GameLib/Framework.h"
#include "GameLib/PseudoXml/Document.h"
#include "GameLib/PseudoXml/Element.h"
#include "GameLib/PseudoXml/Attribute.h"
using namespace GameLib;

PseudoXml::Document gDocument;
int gCount;


namespace GameLib{
	void Framework::configure( Configuration* ){}
	void Framework::update(){
		setFrameRate( 60 );
		if ( !gDocument ){
			//ファイルから生成
			gDocument = PseudoXml::Document::create( "data2.txt" );

			//1から作って吐き出してみる
			using namespace PseudoXml;
			Document doc = Document::create();
			Element root = doc.root();
			//根っこに一個加える
			root.setChildNumber( 1 );
			Element cameraData = root.child( 0 );
			cameraData.setName( "Data" );
			//データを二つ入れる
			cameraData.setChildNumber( 2 );

			Element e;
			//一個目
			e = cameraData.child( 0 );
			e.setName( "SomeElement0" );
			e.setAttributeNumber( 3 );
			e.attribute( 0 ).set( "name", "value" ); //文字列値
			e.attribute( 1 ).set( "intValue", 1 ); //int値
			e.attribute( 2 ).set( "floatValue", 2.32f ); //float値
			//二個目
			e = cameraData.child( 1 );
			e.setName( "SomeElement1" );
			e.setAttributeNumber( 2 );
			int ia[ 5 ];
			for ( int i = 0; i < 5; ++i ){
				ia[ i ] = i;
			}
			e.attribute( 0 ).set( "intArray", ia, 5 ); //int配列
			float fa[ 3 ];
			for ( int i = 0; i < 3; ++i ){
				fa[ i ] = 1.f / static_cast< float >( i + 1 );
			}
			e.attribute( 1 ).set( "floatArray", fa, 3 ); //float配列
			//coutに吐いてみる
			string s;
			doc.convertToString( &s );
			cout << s.c_str() << endl;
			//ファイルにも書き込め
			doc.write( "dataCreated.txt", false );
		}else if ( gDocument.isReady() ){ //読み込み終了
			//文字列化してcoutに出してみる。
			string s;
			gDocument.convertToString( &s );
			cout << s.c_str() << endl;
			//ついでにファイルにも吐いてみる。同じのが出てるか？
			gDocument.write( "dataOut.txt" );
			gDocument.release(); //読み終わったし、いらね。
			requestEnd(); //終わる。
		}
	}
}
