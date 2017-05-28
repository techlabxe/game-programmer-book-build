#include <cmath>

namespace {
	const double r2d = 180.0 / 3.1415926535897932384626433832795;
	const double d2r = 3.1415926535897932384626433832795 / 180.0;
}

namespace GameLib{

double sin( double deg ){
	return std::sin( d2r * deg );
}

double cos( double deg ){
	return std::cos( d2r * deg );
}

double tan( double deg ){
	return std::tan( d2r * deg );
}

double asin( double a ){
	return std::asin( a ) * r2d;
}

double acos( double a ){
	return std::acos( a ) * r2d;
}

double atan( double a ){
	return std::atan( a ) * r2d;
}

double atan2( double y, double x ){
	return std::atan2( y, x ) * r2d;
}

double sqrt( double a ){
	return std::sqrt( a );
}

} //namespace 