#include "GameLib/Framework.h"
#include "GameLib/Base/OStringStream.h"
#include "GameLib/Base/RefString.h"
#include "GameLib/Base/Conversion.h"
#include <cmath>
#include <limits>
using namespace std;

namespace GameLib{
	void Framework::configure( Configuration* ){
	}
	void Framework::update(){

		OStringStream o;
		o << "aho" << endl; //const char*
		char ts[ 3 ];
		ts[ 0 ] = 'O';
		ts[ 1 ] = 'K';
		ts[ 2 ] = '\0';
		o << ts << endl; //char*
		o << string( "baka" ) << endl; //string
		o << RefString( "tonma" ) << endl; //RefString
		o << 'a' << endl; //char
		o << '\n'; //char ( new-line )
		o << static_cast< char >( -74 ) << endl; //char (out of range )
		o << static_cast< char >( 14 ) << endl; //char (out of range )
		o << static_cast< unsigned char >( 99 ) << endl; //unsigned char
		o << static_cast< short >( -9999 ) << endl; //short
		o << static_cast< unsigned short >( 65534 ) << endl; //unsigned short
		o << static_cast< int >( -2012345678 ) << endl; //int
		o << static_cast< unsigned >( 4012345678 ) << endl; //unsigned

		o.precision( 7 );
		o << 50.00000000000 << endl;
		o << 0.09999999999f << endl;
		o << 0.0399999999f << endl;
		o << 1.f << endl; //float ( 1 )
		o << 1.5f << endl; //float ( 1 )
		o << 1.75f << endl; //float ( 1 )
		o << 152423.f << endl; //float ( 1 )
		o << -123456789.f * 1000000.f << endl; //float (big)
		o << -1.23456789f * 0.000000001f << endl; //float (small)
//		o.precision( 15 );
		o << -1.23456789f * 0.000000001f << endl; //float (small)
o << -134.56712839f << endl;
o << -0.056712839f << endl;
		o << 0.0 << endl; //float +0
		o << -0.0 << endl; //float -0
		o << sqrtf( -1.f ) << endl; //float NaN
		o << -1e30f * 1e30f << endl; //float +Inf
		o << 1e-30f * 1e-30f << endl; //float -Denorm
		o << -numeric_limits< float >::max() << endl; //float max
		o << numeric_limits< float >::min() << endl; //float min

		o << -123456789.0 * 1000000.0 << endl; //double (big)
		o << -1.234567890 * 0.000000001 << endl; //double (small)
		o << 0.0 << endl; //double +0
		o << -0.0 << endl; //double -0
		o << sqrt( -1.0 ) << endl; //double NaN
		o << -1e300 * 1e300 << endl; //double +Inf
		o << 1e-300 * 1e-300 << endl; //double -Denorm
		o << -numeric_limits< double >::max() << endl; //double max

		Array< char > out;
		o.get( &out );
		cout << &out[ 0 ] << endl;

		cout << o << endl;

		requestEnd();
	}
}
