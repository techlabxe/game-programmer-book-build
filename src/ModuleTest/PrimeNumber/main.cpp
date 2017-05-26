#include "GameLib/Framework.h"
#include "GameLib/Base/PrimeNumber.h"

//素数生成プログラムのテスト
namespace GameLib{
	using namespace GameLib::PrimeNumber;

	void Framework::configure( Configuration* ){
	}
	void Framework::update(){
		for ( int i = 0; i < 30000; ++i ){
			int a = previous( i );
			int b = next( i );
			ASSERT( i < 3 || a < i );
			ASSERT( b > i );
			ASSERT( isPrimeNumber( a ) );
			ASSERT( isPrimeNumber( b ) );
			for ( int j = 2; j < a; ++j ){
				ASSERT( a % j != 0 );
			}
			for ( int j = 2; j < b; ++j ){
				ASSERT( b % j != 0 );
			}
			cout << a << "\t" << i << "\t" << b << endl;
		}
		requestEnd();
	}
}
