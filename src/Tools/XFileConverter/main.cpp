#include "XFile/Document.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "conversion.h"
#include "GameLib/GameLib.h"
using namespace std;
using namespace GameLib;
using namespace GameLib::PseudoXml;

bool convert( string* dst, const char* src, int size );

int main( int argc, char** argv ){
	bool verbose = false;
	bool help = false;
	vector< string > inFileNames;

	//コマンドライン解釈
	for ( int i = 1; i < argc; ++i ){
		if ( argv[ i ][ 0 ] == '-' ){ //-からはじまればなにかのオプション
			char c = argv[ i ][ 1 ];
			if ( c == 'v' ){ //饒舌モード
				verbose = true;
			}else if ( c == 'h' ){ //ヘルプ
				help = true;
			}
		}else{
			inFileNames.push_back( argv[ i ] );
		}
	}
	//ファイル一個もねえよ
	if ( inFileNames.size() == 0 ){
		help = true;
	}
	//ヘルプ表示して終わり
	if ( help ){
		std::cout << "Usage: XFileConverter.exe [ -v ][ -h ] inputFiles..." << endl;
		std::cout << "<Options>" << endl;
		std::cout << "\t-v             : verbose mode." << endl;
		std::cout << "\t-h             : display this help." << endl;
#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
		return 1;
	}
	//処理始めるよー。
	for ( unsigned i = 0; i < inFileNames.size(); ++i ){
		setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
		ifstream in( inFileNames[ i ].c_str(), ifstream::binary );
		in.seekg( 0, ifstream::end );
		int size = static_cast< int >( in.tellg() );
		in.seekg( 0, ifstream::beg );
		char* src = NEW char[ size + 1 ];
		in.read( src, size );
		src[ size ] = '\0'; //NULL終端
		string dst;
		if ( !convert( &dst, src, size ) ){
			std::cout << "Conversion Failed : " << inFileNames[ i ] << endl;
		}else{
			string outName = inFileNames[ i ] + ".txt";
			setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
			ofstream out( outName.c_str(), ofstream::binary );
			out.write( dst.c_str(), static_cast< streamsize >( dst.size() ) );
			if ( out ){
				std::cout << "Conversion Succeeded : " << inFileNames[ i ] << endl;
			}else{
				std::cout << "Output Failed : " << inFileNames[ i ] << endl;
			}
		}
	}
#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
	return 0;
}

#include <ctime>
bool convert( string* dst, const char* src, int size ){
	//Xファイルデータ構築
	XFile::Document xFile( src, size );
	if ( !xFile.isValid() ){
		return false;
	}
	string xml;
	xFile.convertToString( &xml );
	//ニセxmlとして読み込み
	Document srcDoc = Document::create( xml.c_str(), static_cast< int >( xml.size() ) );
	Document dstDoc = convert( srcDoc );
	
	dstDoc.convertToString( dst );
	return true;
}

#include "GameLib/Framework.h"
namespace GameLib{
	void Framework::configure( Configuration* ){}
	void Framework::update(){}
}


