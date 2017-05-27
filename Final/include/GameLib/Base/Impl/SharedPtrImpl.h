#ifndef INCLUDED_GAMELIB_SHAREDPTRIMPL_H
#define INCLUDED_GAMELIB_SHAREDPTRIMPL_H

namespace GameLib{

template< class T > inline SharedPtr< T >::SharedPtr() : mReference( 0 ){
}

template< class T >	inline SharedPtr< T >::SharedPtr( T* p ){
	if ( p ){
		mReference = NEW Reference( p );
	}
}

template< class T >	inline SharedPtr< T >::SharedPtr( const SharedPtr< T >& p ) : 
mReference( p.mReference ){
	if ( mReference ){
		mReference->increment();
	}
}
template< class T >	inline SharedPtr< T >::~SharedPtr(){
	if ( mReference ){
		if ( mReference->decrement() == 0 ){
			delete mReference->mPointer;
			delete mReference;
		}
		mReference = 0; //いずれにせよポインタは0にする。
	}
}

template< class T >	inline SharedPtr< T >& SharedPtr< T >::operator=( T* p ){
	if ( mReference ){
		if ( mReference->decrement() == 0 ){
			delete mReference->mPointer;
			if ( p ){ //ポインタが空でなければ
				mReference->mPointer = p; //他に誰も使ってないのでこれを書き換えて流用。
				mReference->mCount = 1;
			}else{ //誰も使わないので消す
				delete mReference;
				mReference = 0;
			}
		}else if ( p ){ //ポインタが空でないので新たに生成。だが、referenceは他から見ているので触らない。
			mReference = NEW Reference( p );
		}else{ //ポインタが空。referenceは他から見ているので触らず、参照関係を切る。
			mReference = 0;
		}
	}else if ( p ){
		mReference = NEW Reference( p );
	}
	return *this;
}

template< class T >	inline SharedPtr< T >& SharedPtr< T >::operator=( const SharedPtr< T >& p ){
	if ( mReference ){
		if ( mReference->decrement() == 0 ){ //違う時は0になったら開放
			delete mReference->mPointer;
			delete mReference;
			mReference = 0;
		}
	}
	//差し替え
	mReference = p.mReference;
	if ( mReference ){ //モノがあればインクリメント
		mReference->increment();
	}
	return *this;
}

template< class T >	inline T& SharedPtr< T >::operator*(){
	T* p = ( mReference ) ? mReference->mPointer : 0;
	ASSERT( p );
	return *p;
}

template< class T >	inline const T& SharedPtr< T >::operator*() const { 
	T* p = ( mReference ) ? mReference->mPointer : 0;
	ASSERT( p );
	return *p;
}

template< class T >	inline T* SharedPtr< T >::operator->(){ 
	return ( mReference ) ? mReference->mPointer : 0;
}

template< class T >	inline const T* SharedPtr< T >::operator->() const { 
	return ( mReference ) ? mReference->mPointer : 0;
}

//ポインタをそのまま返すと悪用される恐れがあるので、非ゼロなら0xffffffffを返す。
template< class T >	inline SharedPtr< T >::operator void*() const {
	T* p = ( mReference ) ? mReference->mPointer : 0;
	return ( p ) ? reinterpret_cast< void* >( ~0 ) : 0;
}

//Reference
template< class T > inline SharedPtr< T >::Reference::Reference( T* p ) :
mPointer( p ),
mCount( 1 ){
}

template< class T > inline SharedPtr< T >::Reference::~Reference(){
	ASSERT( mCount == 0 );
	mPointer = 0;
}

template< class T > inline void SharedPtr< T >::Reference::increment(){
	++mCount;
}

template< class T > inline int SharedPtr< T >::Reference::decrement(){
	--mCount;
	return mCount;
}


//以下const版
template< class T > inline ConstSharedPtr< T >::ConstSharedPtr(){
}

template< class T >	inline ConstSharedPtr< T >::ConstSharedPtr( const SharedPtr< T >& p ) : 
mPointer( p ){
}

template< class T >	inline ConstSharedPtr< T >::~ConstSharedPtr(){}

template< class T >	inline ConstSharedPtr< T >& ConstSharedPtr< T >::operator=( const SharedPtr< T >& p ){
	mPointer = p;
	return *this;
}

template< class T >	inline const T& ConstSharedPtr< T >::operator*() const { 
	return mPointer.operator*();
}

template< class T >	inline const T* ConstSharedPtr< T >::operator->() const { 
	return mPointer.operator->();
}

template< class T >	inline ConstSharedPtr< T >::operator void*() const {
	return mPointer.operator void*();
}

} //namespace GameLib

#endif