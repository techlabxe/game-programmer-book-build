#include <iostream>
using namespace std;

int main(){
	const int n = 2500 * 10000;
	int* a = new int[ n ];
	//適当にソートされたデータを生成
	for ( int i = 0; i < n; ++i ){
		a[ i ] = i * 5;
	}
	cout << "SEARCH BEGIN" << endl;

	int loopCount = 0;
	//探す値はこんな感じで
	int search = 89012345;
	//最初の検索範囲
	int first = 0;
	int last = n - 1;
	while ( last > first ){ //1個以上あるうちは繰り返す
		int middle = ( first + last ) / 2; //真ん中の添え字
		cout << loopCount << "\t" << first << "-" << last << "\tmiddle=" << middle << "\trange=" << last-first << endl;
		++loopCount;
		int t = a[ middle ];
		if ( t < search ){ //小さい。後ろにある
			first = middle + 1; //先頭を置換
		}else if ( t > search ){ //大きい。前にある。
			last = middle - 1; //末尾を置換
		}else{
			first = last = middle;
		}
	}
	//ここまでくればfirst == lastだ。
	if ( a[ first ] == search ){
		cout << "Find! " << first << endl;
	}else{
		cout << "Not Find!" << endl;
	}
	delete[] a;

	//無限ループ
	while( true );



	return 0;
}