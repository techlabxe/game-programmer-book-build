#include <windows.h>
#include <string>
#include <list>
#include <fstream>
#include <iostream>
using namespace std;
/*
このサンプルではwindowsの関数のASCII版(普通のchar*を取るバージョン)
を直接呼び出して使う。本来はTCHARなどのマクロを多用してUnicodeとASCIIを
意識せずに使うのが作法なのだが、見慣れないコードは読みにくいし、
別にwindowsプログラミングを学ぶことは目的ではない。
実際perlやbash、C#などを覚えればこの手のツールをC++で書くこともなくなるだろう。
ここではwindows作法を無視してASCII版を直接呼ぶ。

<使う関数>
GetFirstFileA
GetNextFileA
<使う構造体>
WIN32_FIND_DATAA
*/

//ファイル一覧を取得
void enumerateFiles( 
list< string >& fileNameListOut, 
const string& directoryName );

//int書き込みの便利関数
void write( ofstream* o, int a ){
   char str[ 4 ];
   str[ 0 ] = static_cast< char >( ( a & 0x000000ff ) >>  0 );
   str[ 1 ] = static_cast< char >( ( a & 0x0000ff00 ) >>  8 );
   str[ 2 ] = static_cast< char >( ( a & 0x00ff0000 ) >> 16 );
   str[ 3 ] = static_cast< char >( ( a & 0xff000000 ) >> 24 );
   o->write( str, 4 );
}


//アーカイブを作る
void createArchive( 
const string* fileNames, 
int fileNumber, 
const char* archiveName );

//第一引数が出力ファイル名
int main( int, char** argv ){
	//よく使うものに別名を
	typedef list< string > List;
	typedef List::iterator It;

	//アーカイブ名は第一引数から
	string archiveName = argv[ 1 ];
	archiveName += ".bin"; //バイナリファイルで勝手に作ったものはとりあえずbinあたりの拡張子を付けることが多い。

	List fileNameList;
	//ファイルリストを作る
	enumerateFiles( fileNameList, argv[ 1 ] );
	//使いにくいので配列に変換
	int fileNumber = static_cast< int >( fileNameList.size() );
	string* fileNames = new string[ fileNumber ];
	int j = 0;
	for ( It i = fileNameList.begin(); i != fileNameList.end(); ++i ){
		fileNames[ j ] = *i;
		++j;
	}

	//アーカイブ生成
	createArchive( fileNames, fileNumber, archiveName.c_str() );

#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
	return 0;
}

//------------------------関数----------------------------

void enumerateFiles( 
list< string >& fileNameList, 
const string& directoryName ){
	HANDLE iterator; //イテレータみたいなものなのでiteratorと名を付けてみる。
	WIN32_FIND_DATAA fileData; //ここにファイル名が入れられる。
	//最初のファイル取得
	string searchPath = directoryName;
	searchPath += "\\*.*"; //\は特殊な文字なので二個重ねる。そうするとちょうど一個入る。
	//生ファイル。ディレクトリは後で。
	iterator = FindFirstFileA( searchPath.c_str(), &fileData );
	while ( true ){ //とりあえず条件なし。ループの後ろで抜ける
		const char* name = fileData.cFileName;
		if ( !( fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ){ //ディレクトリは無視
			//ファイル名にはディレクトリ名込みにしてやる必要がある
			string filename = directoryName;
			filename += '\\';
			filename += name; 
			cout << "\tFile : " << filename << endl; //表示してやろう。デバグにもなるし。
			fileNameList.push_back( filename ); //リストに追加
		}
		if ( !FindNextFileA( iterator, &fileData ) ){ //次へ。次がなければ抜ける。iteratorが変更されそうにないが、HANDLE型はポインタなので、これで大丈夫。
			break;
		}
	}
	//今度はディレクトリだけ
	iterator = FindFirstFileA( searchPath.c_str(), &fileData );
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
			cout << "Directory : " << newDirectoryName << endl; //表示してやろう。デバグにもなるし。
			enumerateFiles( fileNameList, newDirectoryName.c_str() ); //再帰呼び出し
		} //ディレクトリでなければ何もしない
		if ( !FindNextFileA( iterator, &fileData ) ){ //次へ。次がなければ抜ける。iteratorが変更されそうにないが、HANDLE型はポインタなので、これで大丈夫。
			break;
		}
	}
}

void createArchive( const string* fileNames, int fileNumber, const char* archiveName ){
	//書き込み先を開けて
	ofstream out( archiveName, ofstream::binary );
	//ファイルサイズ配列を確保
	int* fileSizes = new int[ fileNumber ];

	//リストfileNamesを見て開けては読み出してコピー
	for ( int i = 0; i < fileNumber; ++i ){
		ifstream in( fileNames[ i ].c_str(), ifstream::binary );
		in.seekg( 0, ifstream::end );
		fileSizes[ i ] = in.tellg();
		in.seekg( 0, ifstream::beg );
		char* data = new char[ fileSizes[ i ] ];
		in.read( data, fileSizes[ i ] ); //読み
		out.write( data, fileSizes[ i ] ); //書き
	}
	//ファイルの末尾の位置を記録
	int dataEnd = out.tellp(); //tellgはgetの位置で、tellpはputの位置だ
	//まずファイルの数を書き込んでやろう
	write( &out, fileNumber );
	//表を作るためにサイズをオフセットに直しながら、
	//場所、サイズ、名前の文字数、名前を格納していく。
	int pos = 0;
	for ( int i = 0; i < fileNumber; ++i ){
		write( &out, pos );
		write( &out, fileSizes[ i ] );
		string name = fileNames[ i ].c_str();
		int nameLength = static_cast< int >( name.size() );
		write( &out, nameLength );
		out.write( name.c_str(), nameLength );
		cout << pos << '\t' << fileSizes[ i ] << '\t' << nameLength << '\t' << name << endl; //デバグ用に表示してやろう
		pos += fileSizes[ i ];
	}
	//最後に表の開始位置を書き込んでやる。
	write( &out, dataEnd );
	//後始末
	delete[] fileSizes; //SAFE_DELETE_ARRAYはないので。私のライブラリじゃないから。
	fileSizes = 0;
}
