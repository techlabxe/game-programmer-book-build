#include <algorithm> //random_shuffle()のために
#include <iostream>
#include <ctime> //clock()のために
using namespace std;

int main( int argc, char** ){
	cout << "PREPARING... " << endl;

	const int N = 1000 * 1000 * 20; //10MB分
	int* a = new int[ N ]; 
	int** p0 = new int*[ N ];
	int** p1 = new int*[ N ];
	for ( int i = 0; i < N; ++i ){
		a[ i ] = i;
		p0[ i ] = &a[ i ];
		p1[ i ] = &a[ i ];
	}
	//p1をバラバラに
	random_shuffle( p1, p1 + N );
	//argcを見てどっちのポインタ配列を使うか選択
	int** p = ( argc == 1 ) ? p0 : p1;

	double t0 = clock();	//時間始点
	//100回アクセス
	for ( int j = 0; j < 100; ++j ){
		for ( int i = 0; i < N; ++i ){
			++( *p[ i ] );
		}
		cout << j << endl;
	}
	double t1 = clock(); //時間終点

	cout << "TIME: " << ( t1 - t0 ) / CLOCKS_PER_SEC << endl; //これで秒になる

	while( true ); //無限ループ

	return 0;
}
