#include <windows.h>
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
using namespace std;
/*
このサンプルではwindowsの関数のASCII版(普通のchar*を取るバージョン)
を直接呼び出して使う。本来はTCHARなどのマクロを多用してUnicodeとASCIIを
意識せずに使うのが作法なのだが、見慣れないコードは読みにくいし、
別にwindowsプログラミングを学ぶことは目的ではない。
実際perlやbash、C#などを覚えればこの手のツールをC++で書くこともなくなるだろう。
ここではwindows作法を無視してASCII版を直接呼ぶ

<使う関数>
GetFirstFileA
GetNextFileA
<使う構造体>
WIN32_FIND_DATAA
*/

//ファイル一覧を取得
void enumerateFiles( 
	list< string >& fileNameListOut, 
	const string& directoryName,
	bool verbose );

//アーカイブを作る
bool createArchive( 
	const string* fileNames, 
	int fileNumber, 
	const char* archiveName,
	int blockSize,
	bool verbose,
	double compressionThreshold );

//圧縮本体
bool compress( 
	int* outSize, 
	int* inPlaceDecompressionBufferSize,
	bool* compressed,
	ofstream* oStream, 
	ifstream* iStream,
	double threshold );

//int書き込みの便利関数
void write( ofstream* o, unsigned a ){
   char str[ 4 ];
   str[ 0 ] = static_cast< char >( ( a & 0x000000ff ) >>  0 );
   str[ 1 ] = static_cast< char >( ( a & 0x0000ff00 ) >>  8 );
   str[ 2 ] = static_cast< char >( ( a & 0x00ff0000 ) >> 16 );
   str[ 3 ] = static_cast< char >( ( a & 0xff000000 ) >> 24 );
   o->write( str, 4 );
}

#include "GameLib/GameLib.h"

//第一引数が出力ファイル名
int main( int argc, char** argv ){
	int blockSize = 4;
	string archiveDir;
	string archiveName;
	bool verbose = false;
	bool help = false;
	double threshold = 100.0;
	//コマンドライン解釈
	for ( int i = 1; i < argc; ++i ){
		if ( argv[ i ][ 0 ] == '-' ){ //-からはじまればなにかのオプション
			char c = argv[ i ][ 1 ];
			if ( c == 'b' ){ //サイズ指定
				if ( ( i + 1 ) < argc ){
					istringstream iss( argv[ i + 1 ] );
					iss >> blockSize;
				}
				++i;
			}else if ( c == 'o' ){ //出力ファイル名指定
				if ( ( i + 1 ) < argc ){
					archiveName = argv[ i + 1 ];
				}
				++i;
			}else if ( c == 'r' ){ //閾値
				if ( ( i + 1 ) < argc ){
					istringstream iss( argv[ i + 1 ] );
					iss >> threshold;
					if ( threshold > 100.0 ){
						threshold = 100.0; //100以上は許さない
					}
				}
				++i;
			}else if ( c == 'v' ){ //饒舌モード
				verbose = true;
			}else if ( c == 'h' ){ //ヘルプ
				help = true;
			}
		}else{
			archiveDir = argv[ i ];
		}
	}
	//アーカイブディレクトリ指定がないと駄目。
	if ( archiveDir.size() == 0 ){
		help = true;
	}
	//ヘルプ表示して終わり
	if ( help ){
		cout << "Usage: Archiver.exe rootDir [ -o outfilename ][ -s blockSize ][ -r thresholdRatio ][ -v ][ -h ]" << endl;
		cout << "<Options>" << endl;
		cout << "\t-o outfilename : specify output archive name. default is \"rootDir\".bin" << endl;
		cout << "\t-s blocksize   : specify block size. \"4GB * blockSize\" is maximum archivesize. default is 4." << endl;
		cout << "\t-r percent     : specify threshold. if compressed size is less than this, non-compressed data is written." << endl;
		cout << "\t-v             : verbose mode." << endl;
		cout << "\t-h             : display this help." << endl;
#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
		return 1;
	}
	//アーカイブ名がなければアーカイブディレクトリ名からつけてやる
	if ( archiveName.size() == 0 ){
		archiveName = archiveDir;
		archiveName += ".bin";
	}

	//よく使うものに別名を
	typedef list< string > List;
	typedef List::iterator It;
	List fileNameList;
	//ファイルリストを作る
	enumerateFiles( fileNameList, archiveDir.c_str(), verbose );
	//使いにくいので配列に変換
	int fileNumber = static_cast< int >( fileNameList.size() );
	string* fileNames = new string[ fileNumber ];
	int j = 0;
	for ( It i = fileNameList.begin(); i != fileNameList.end(); ++i ){
		fileNames[ j ] = *i;
		++j;
	}

	//アーカイブ生成
	bool succeeded = createArchive( 
		fileNames, 
		fileNumber, 
		archiveName.c_str(), 
		blockSize, 
		verbose, 
		threshold );
	if ( !succeeded ){ //失敗時消したりはしない。した方がいいんだろうが。
		cerr << "creating failed. delete output file manually." << endl;
#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
		return 1;
	}

#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
	return 0;
}

//------------------------関数----------------------------

void enumerateFiles( 
list< string >& fileNameList, 
const string& directoryName,
bool verbose ){
	HANDLE iterator; //イテレータみたいなものなのでiteratorと名を付けてみる。
	WIN32_FIND_DATAA fileData; //ここにファイル名が入れられる。
	//最初のファイル取得
	string searchPath = directoryName;
	searchPath += "\\*.*"; //\は特殊な文字なので二個重ねる。そうするとちょうど一個入る。

	//ファイルだけ
	iterator = FindFirstFileA( searchPath.c_str(), &fileData );
	if ( iterator == INVALID_HANDLE_VALUE ){
		//何か失敗した。
		return;
	}
	while ( true ){ //とりあえず条件なし。ループの後ろで抜ける
		if ( !( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ){
			//ファイル名にはディレクトリ名込みにしてやる必要がある
			string filename = directoryName;
			filename += '\\';
			filename += fileData.cFileName; 
			if ( verbose ){
				cout << "\tFile : " << filename << endl; //表示してやろう。デバグにもなるし。
			}
			fileNameList.push_back( filename ); //リストに追加
		}
		if ( !FindNextFileA( iterator, &fileData ) ){ //次へ。次がなければ抜ける。iteratorが変更されそうにないが、HANDLE型はポインタなので、これで大丈夫。
			break;
		}
	}
	//ディレクトリだけ
	iterator = FindFirstFileA( searchPath.c_str(), &fileData );
	if ( iterator == INVALID_HANDLE_VALUE ){
		//何か失敗した。
		return;
	}
	while ( true ){ //とりあえず条件なし。ループの後ろで抜ける
		string name = fileData.cFileName;
		//困ったことに.とか..とかも返すので、こいつらは除外。でないと無限ループしてしまう。
		if ( name == "." || name == ".." ){
			; //何もしない
		}else if ( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){ //ディレクトリです。
			//ディレクトリ名を継ぎ足してやる。
			string newDirectoryName = directoryName;
			newDirectoryName += '\\';
			newDirectoryName += name;
			if ( verbose ){
				cout << "Directory : " << newDirectoryName << endl;
			}
			enumerateFiles( fileNameList, newDirectoryName.c_str(), verbose ); //再帰呼び出し
		}
		if ( !FindNextFileA( iterator, &fileData ) ){ //次へ。次がなければ抜ける。iteratorが変更されそうにないが、HANDLE型はポインタなので、これで大丈夫。
			break;
		}
	}
}

bool createArchive( 
const string* fileNames, 
int fileNumber, 
const char* archiveName, 
int blockSize, 
bool verbose,
double threshold ){
	//書き込み先を開けて
	ofstream out( archiveName, ofstream::binary );
	if ( !out ){
		cerr << "can't create output file." << endl;
		return false;
	}
	//ファイルサイズ配列を確保
	int* originalSizes = new int[ fileNumber ];
	int* outputSizes = new int[ fileNumber ];
	int* inPlaceDecompressionBufferSizes = new int[ fileNumber ];

	//リストfileNamesを見て開けては読み出してコピー
	for ( int i = 0; i < fileNumber; ++i ){
		setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
		ifstream in( fileNames[ i ].c_str(), ifstream::binary );
		if ( !in ){
			cerr << "can't open file : " << fileNames[ i ] << endl;
			return false;
		}
		in.seekg( 0, ifstream::end );
		streamsize sizeBig = in.tellg();
		if ( sizeBig >= 0x7fffffff ){
			cerr << "can't handle files larger than 2GB!" << endl;
			delete[] outputSizes;
			outputSizes = 0;
			delete[] originalSizes;
			originalSizes = 0;
			out.close();
			out.open( archiveName ); //開きなおす事で中身を空に
			return false;
		}
		in.seekg( 0, ifstream::beg );
		originalSizes[ i ] = static_cast< int >( sizeBig );
		bool compressed;
		compress( &outputSizes[ i ], &inPlaceDecompressionBufferSizes[ i ], &compressed, &out, &in, threshold );
		if ( verbose ){
			double ratio = ( sizeBig == 0 ) ? 100.0 : outputSizes[ i ] * 100.0 / sizeBig;
			cout << fileNames[ i ] << " ) : " << sizeBig << " -> " << outputSizes[ i ] << " ( " << ratio << "% )" << endl;
		}
		//ブロックサイズあまりにダミーを書き込み
		int mod = outputSizes[ i ] % blockSize;
		int padding = ( mod > 0 ) ? ( blockSize - mod ) : 0;
		for ( int j = 0; j < padding; ++j ){
			out.put( '\0' );
		}
	}
	if ( verbose ){
		cout << "<Table>" << endl;
		cout << "FileNumber : " << fileNumber << endl;
		cout << "BlockSize : " << blockSize << endl;
	}
	//ファイルの末尾の位置を記録
	streamsize dataEnd = out.tellp(); //tellgはgetの位置で、tellpはputの位置だ
	//まずファイルの数を書き込んでやろう
	out.write( reinterpret_cast< char* >( &fileNumber ), 4 );
	//表を作るためにサイズをオフセットに直しながら、
	//場所、サイズ、名前の文字数、名前を格納していく。
	unsigned pos = 0; //ブロック単位の位置
	for ( int i = 0; i < fileNumber; ++i ){
		write( &out, pos );
		write( &out, outputSizes[ i ] );
		write( &out, originalSizes[ i ] );
		write( &out, inPlaceDecompressionBufferSizes[ i ] );
		string name = fileNames[ i ].c_str();
		int nameLength = static_cast< int >( name.size() );
		write( &out, nameLength );
		out.write( name.c_str(), nameLength );
		if ( verbose ){
			cout << pos << '\t' << outputSizes[ i ] << '\t' << originalSizes[ i ] << '\t' << inPlaceDecompressionBufferSizes[ i ] << '\t' << nameLength << '\t' << name << endl; //デバグ用に表示してやろう
		}
		pos += ( outputSizes[ i ] + blockSize - 1 ) / blockSize;
	}
	//最後に書き込むのがブロックサイズ
	write( &out, blockSize );
	//表の開始位置を書き込んでやる。
	assert( dataEnd % blockSize == 0 );
	unsigned dataEndInBlock = static_cast< unsigned >( dataEnd / blockSize );
	write( &out, dataEndInBlock );
	//後始末
	delete[] outputSizes;
	outputSizes = 0;
	delete[] originalSizes;
	originalSizes = 0;
	delete[] inPlaceDecompressionBufferSizes;
	inPlaceDecompressionBufferSizes = 0;

	return true;
}
