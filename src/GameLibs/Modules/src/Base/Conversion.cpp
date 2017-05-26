#include "GameLib/Base/Conversion.h"

namespace GameLib{

namespace {

int toString16( char* out, unsigned a, int n ){ //nは16進化した時の最大桁数
	static const char table[ 16 ] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F', };
	for ( int i = 0; i < n; ++i ){ //上の桁から順に処理
		int shift = ( n - 1 - i ) * 4;
		unsigned mask = 0xf << shift;
		unsigned t = a & mask;
		t >>= shift;
		out[ i ] = table[ t ];
	}
	return n;
}

//先頭の0をそのままにして返す。
//何故マイナス判定を中でやらないかと言えば、unsignedで受け取れば
//マイナスのintが入らないし、intで受け取ればunsignedが入らないからである
void toString10Core( char* out,  unsigned a, int n ){ //nは10進化した時の最大桁数。最大10
	//10進定数
	static const int d[ 9 ] = { 
		10,
		100,
		1000,
		10000,
		100000,
		1000000,
		10000000,
		100000000,
		1000000000,
	};
	//10で割ったあまりを格納して、10で割る。
	for ( int i = 0; i < n - 1; ++i ){
		int q = a / d[ n - 2 - i ];
		out[ i ] = static_cast< char >( q );
		a -= q * d[ n - 2 - i ];
	}
	out[ n - 1 ] = static_cast< char >( a );
}

//何故マイナス判定を中でやらないかと言えば、unsignedで受け取れば
//マイナスのintが入らないし、intで受け取ればunsignedが入らないからである
int toString10( char* out,  unsigned a, int n, bool minus ){ //nは10進化した時の最大桁数。最大10
	char* p = out;
	//マイナスならマイナスを吐き出す
	if ( minus ){
		*p++ = '-';
	}
	char s[ 10 ]; //中間バッファ
	toString10Core( s, a, n );
	//0をスキップ
	int begin = n;
	for ( int i = 0; i < n; ++i ){
		if ( s[ i ] != 0 ){
			begin = i;
			break;
		}
	}
	if ( begin == n ){ //一桁もない。つまり0
		*p++ = '0';
	}else{
		for ( int i = begin; i < n; ++i ){
			*p++ = '0' + s[ i ];
		}
	}
	return static_cast< int >( p - out );
}

} //namespace {}

int toString10( char* out, char a ){
	bool minus = false;
	if ( a < 0 ){
		minus = true;
		a = -a;
	}
	return toString10( out, a, 3, minus );
}

int toString10( char* out, unsigned char a ){
	return toString10( out, a, 3, false );
}

int toString10( char* out, short a ){
	bool minus = false;
	if ( a < 0 ){
		minus = true;
		a = -a;
	}
	return toString10( out, a, 5, minus );
}

int toString10( char* out, unsigned short a ){
	return toString10( out, a, 5, false );
}

int toString10( char* out, int a ){
	bool minus = false;
	if ( a < 0 ){
		minus = true;
		a = -a;
	}
	return toString10( out, a, 10, minus );
}

int toString10( char* out, unsigned a ){
	return toString10( out, a, 10, false );
}

int toString16( char* out, char a ){
	return toString16( out, a, 2 );
}

int toString16( char* out, unsigned char a ){
	return toString16( out, a, 2 );
}

int toString16( char* out, short a ){
	return toString16( out, a, 4 );
}

int toString16( char* out, unsigned short a ){
	return toString16( out, a, 4 );
}

int toString16( char* out, int a ){
	return toString16( out, a, 8 );
}

int toString16( char* out, unsigned a ){
	return toString16( out, a, 8 );
}

int toString( char* out, float a, int precision ){
	char* p = out; //書き込みポインタ
	if ( a < 0.f ){ //符号処理
		*p++ = '-';
		a = -a;
	}
	unsigned o = *reinterpret_cast< unsigned* >( &a );
	int e = ( o & 0x7f800000 ) >> 23;
	unsigned m = o & 0x007fffff; //隠れビットの1を足す
	//特殊数を処理
	if ( e == 0 ){
		*p++ = '0';
		return static_cast< int >( p - out );
	}else if ( e == 255 ){
		if ( m == 0 ){
			*p++ = 'I';
			*p++ = 'n';
			*p++ = 'f';
		}else{
			*p++ = 'N';
			*p++ = 'a';
			*p++ = 'N';
		}
		return static_cast< int >( p - out );
	}
	e -= 127; //指数バイアス
	e -= 23; //仮数部の2^23ケタ分を指数から引く
	m += 0x00800000; //隠れビットの1を足す
	m <<= 8; //8ビットシフト
	e -= 8; //その分桁ダウン

	int e10 = 0;
	if ( e > 0 ){ //2冪を乗算しつつ10で除する。
		for ( int i = 0; i < e; ++i ){
			if ( m > 0x80000000 ){ //あふれる！
				m /= 10;
				++e10;
				if ( i + 2 < e ){ //まだ2回以上乗ずるならここでやってしまう。
					m <<= 2;
					i += 2;
				}
			}
			m <<= 1;
		}
	}else{
		e = -e;
		for ( int i = 0; i < e; ++i ){
			if ( m < 0x19000000 ){ //ケタを上げます
				m *= 10;
				--e10;
				if ( i + 2 < e ){ //まだ2回以上除するならここでやってしまう。
					m >>= 2;
					i += 2;
				}
			}
			m >>= 1;
		}
	}
	char s[ 10 ]; //中間吐き出しバッファ
	toString10Core( s, m, 10 );
	//以下フォーマッティング

	//先頭が0でないようにずらす。後の工程が単純化する
	int zeroEnd = 0;
	for ( int i = 0; i < 10; ++i ){
		if ( s[ i ] != 0 ){
			break;
		}else{
			++zeroEnd;
		}
	}
	if ( zeroEnd == 10 ){ //全部0特殊な場合なので抜ける
		*p++ = '0';
		return static_cast< int >( p - out );
	}
	//移す
	for ( int i = 0; i < 10 - zeroEnd; ++i ){
		s[ i ] = s[ i + zeroEnd ];
	}
	for ( int i = 10 - zeroEnd; i < 10; ++i ){
		s[ i ] = 0;
	}
	e10 -= zeroEnd; //上にずらした分桁減少
	e10 += 9; //10億の位を1にするので9ケタeにプラス

	//出力桁数
	int n = ( precision < 9 ) ? precision : 9;
	//四捨五入
	bool carry = ( s[ n ] >= 5 );
	s[ n ] = 0;
	if ( carry ){
		for ( int i = n - 1; i >= 0; --i ){
			++s[ i ];
			if ( s[ i ] == 10 ){
				s[ i ] = 0;
			}else{
				carry = false;
				break;
			}
		}
		if ( carry ){ //最大桁まで繰り上がっている。極めて稀
			s[ 0 ] = 1;
			for ( int i = 1; i < 10; ++i ){
				s[ i ] = 0;
			}
			++e10;
		}
	}
	//次に、後ろから0を数えてその数だけnを減らす
	for ( int i = n - 1; i > 0; --i ){
		if ( s[ i ] == 0 ){
			--n;
		}else{
			break;
		}
	}
	//e10が[-2,precision)にあればe記法は使わない。その方が短くなるからだ。
	if ( ( e10 >= -2 ) && ( e10 < precision ) ){ 
		if ( e10 >= 0 ){ //第一位が整数の場合
			for ( int i = 0; i < n; ++i ){
				*p++ = '0' + s[ i ];
				if ( ( i == e10 ) && ( i != n - 1 ) ){ //最後の数なら小数点無用
					*p++ = '.'; //小数点出力
				}
			}
			//eがでかい場合は余分の0が必要
			for ( int i = n; i <= e10; ++i ){
				*p++ = '0';
			}
		}else{ //第一位が小数の場合
			*p++ = '0';
			*p++ = '.';
			if ( e10 == -2 ){
				*p++ = '0';
			}
			for ( int i = 0; i < n; ++i ){
				*p++ = '0' + s[ i ];
			}
		}
	}else{ //e記法
		//一桁出力
		*p++ = '0' + s[ 0 ];
		//小数点出力
		*p++ = '.';
		for ( int i = 1; i < n; ++i ){
			*p++ = '0' + s[ i ];
		}
		if ( e10 != 0 ){
			*p++ = 'e';
			if ( e10 < 0 ){
				*p++ = '-';
				e10 = -e10;
			}
			int q = e10 / 10;
			if ( q != 0 ){	
				*p++ = '0' + static_cast< char >( q );
			}
			e10 -= q * 10;
			*p++ = '0' + static_cast< char >( e10 );
		}
	}
	return static_cast< int >( p - out );
}

int toString( char* out, double a, int precision ){
	return toString( out, static_cast< float >( a ), precision ); //TODO
}


} //namespace GameLib

