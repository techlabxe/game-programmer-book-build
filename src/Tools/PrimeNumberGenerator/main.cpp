#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <list>
using namespace std;

int main( int argc, char** argv ){
	string outFileName;
	int m = 0;
	bool help = false;
	//コマンドライン解釈
	for ( int i = 1; i < argc; ++i ){
		if ( argv[ i ][ 0 ] == '-' ){ //-からはじまればなにかのオプション
			char c = argv[ i ][ 1 ];
			if ( c == 'o' && ( ( i + 1 ) < argc ) ){ 
				outFileName = argv[ i + 1 ];
			}else if ( c == 'h' ){ //ヘルプ
				help = true;
			}
		}else{
			istringstream iss( argv[ i ] );
			iss >> m;
			if ( m < 2 ){
				cerr << "MAX_NUMBER must be >= 2.";
				help = true;
			}
		}
	}
	if ( m == 0 ){
		help = true;
	}
	//ヘルプ表示して終わり
	if ( help ){
		cout << "Usage: PrimeNumberGenerator.exe [ -h ][ -o outputFile ] MAX_NUMBER" << endl;
		cout << "<Arguments>" << endl;
		cout << "\t-h             : display this help." << endl;
		cout << "\t-o             : specify output file." << endl;
		cout << "\tMAX_NUMBER     : specify maximum number. must be >= 2" << endl;
#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
#endif
		return 1;
	}
	ostream* out;
	if ( outFileName.size() > 0 ){
		setlocale( LC_ALL, "" ); //これがないと日本語ファイル名を受け付けない
		out = new ofstream( outFileName.c_str() );
	}else{
		out = &cout;
	}
	//素数生成
	//1.2と、max以下の奇数を全部リストに入れる
	list< int > l;
	l.push_back( 2 );
	for ( int i = 3; i <= m; i += 2 ){
		l.push_back( i );
	}
	//2.ルートmaximumを生成
	int n = static_cast< int >( sqrt( static_cast< double >( m ) ) );
	//3ふるい発動
	for ( int i = 3; i < n; ++i ){
		list< int >::iterator j = l.begin();
		while ( j != l.end() ){
			list< int >::iterator next = j;
			++next;
			if ( *j != i ){
				if ( ( *j % i ) == 0 ){ //割れちゃう
					l.erase( j );
				}
			}
			j = next;
		}
	}
	//4出力
	*out << "const int gPrimeNumbers[] = {" << endl;
	list< int >::iterator i = l.begin();
	int j = 0;
	while ( i != l.end() ){
		if ( ( j % 8 ) == 0 ){
			*out << "\t";
		}
		*out << *i << ",\t";
		if ( ( j % 8 ) == 7 ){
			*out << endl;
		}
		++i;
		++j;
	}
	if ( ( j % 8 ) != 7 ){
		*out << endl;
	}
	*out << "};" << endl;
	*out << "const int gPrimeNumberTableSize = " << l.size() << ";" << endl;
	*out << "const int gPrimeNumberTableMaximum = " << m << ";" << endl;
#ifndef NDEBUG //こうしておくとデバグの時しか来ない
	if ( outFileName.size() == 0 ){
		while( true ){;} //visual studioから実行する時にコンソールを良くみたいので。
	}
#endif
	return 0;
}
