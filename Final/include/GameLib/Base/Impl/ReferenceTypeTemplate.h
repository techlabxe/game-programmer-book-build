//これはクラスを書く人向けで、ユーザがインクルードすることはない。
//参照カウント型のクラスでは必ず持っている関数というものがあり、
//これを書くのを楽にする。
//#define TYPE ActualType
//と書いてからインクルードすること。もちろんActualTypeは本当の型の名前だ。

TYPE::TYPE() : mImpl( 0 ){
}

TYPE::TYPE( const TYPE& o ) : mImpl( o.mImpl ){
	if ( mImpl ){
		mImpl->refer();
	}
}

TYPE::~TYPE(){
	release();
}

void TYPE::release(){
	if ( mImpl ){
		mImpl->release();
		if ( mImpl->referenceCount() == 0 ){
			SAFE_DELETE( mImpl );
		}
	}
	mImpl = 0;
}

TYPE& TYPE::operator=( const TYPE& o ){
	release();
	mImpl = o.mImpl;
	if ( mImpl ){
		mImpl->refer();
	}
	return *this;
}

bool TYPE::operator==( const TYPE& o ) const {
	return ( mImpl == o.mImpl );
}

bool TYPE::operator!=( const TYPE& o ) const {
	return ( mImpl != o.mImpl );
}

TYPE::operator void*() const {
	return ( mImpl == 0 ) ? 0 : reinterpret_cast< void* >( ~0 );
}

#undef TYPE
