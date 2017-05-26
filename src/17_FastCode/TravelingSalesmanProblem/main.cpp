#include <iostream>
#include <cstdlib> //rand(),atoi()
#include <limits>
#include <ctime> //clock()
using namespace std;

//場所構造体
struct Position{
	double x;
	double y;
};

//再帰検索本体
void search( 
int* minPath, 
int* path, 
int cn, 
int n, 
const Position* p, 
double currentLength, 
const Position& currentPos );

//最短距離
double gMinLength = numeric_limits< double >::max(); //とても大きな数で初期化

int main( int argc, char** argv ){
	if ( argc < 2 ){
		return 1; //エラー
	}
	int n = atoi( argv[ 1 ] );

	//家の場所を初期化
	Position* positions = new Position[ n ];
	for ( int i = 0; i < n; ++i ){
		positions[ i ].x = rand();
		positions[ i ].y = rand();
		cout << i << " : " << positions[ i ].x << "," << positions[ i ].y << endl;
	}
	//再帰呼び出し開始
	int* minPath = new int[ n ];
	int* path = new int[ n ];
	Position currentPos;
	currentPos.x = currentPos.y = 0.0; //初期位置
	double t0 = clock(); //開始時刻
	search( minPath, path, 0, n, positions, 0.0, currentPos );
	double t1 = clock(); //終了時刻

	cout << "Length : " << gMinLength << endl;
	for ( int i = 0; i < n; ++i ){
		cout << i << " " << minPath[ i ] << endl;
	}
	cout << "Calculation Time : " << ( t1 - t0 ) / CLOCKS_PER_SEC << endl;
	while(1);
	return 0;
}

void search( 
int* minPath, 
int* path, 
int cn, 
int n, 
const Position* p, 
double currentLength, 
const Position& currentPos ){
	for ( int i = 0; i < n; ++i ){
		//行った事がない場所か？
		bool found = false;
		for ( int j = 0; j < cn; ++j ){
			if ( path[ j ] == i ){
				found = true;
				break;
			}
		}
		if ( !found ){ //行った事がないので次はここへ行こう
			path[ cn ] = i;
			double dx = currentPos.x - p[ i ].x;
			double dy = currentPos.y - p[ i ].y;
			double l = currentLength + sqrt( dx * dx + dy * dy );
			if ( cn == n - 1 ){ //最後まで来たので再帰呼び出しせずグローバルと比較。終わる。
				if ( l < gMinLength ){
					gMinLength = l;
					//経路も保存
					for ( int j = 0; j < n; ++j ){
						minPath[ j ] = path[ j ];
					}
					//現在の解を表示
					cout << l << " : ";
					for ( int j = 0; j < n; ++j ){
						cout << path[ j ] << ',';
					}
					cout << endl;
				}
			}else{
				search( minPath, path, cn + 1, n, p, l, p[ i ] );
			}
		}
	}
}
