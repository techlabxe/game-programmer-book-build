#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#endif

#include <cstring> //sprintfなどなど。使いたくないが仕方ない。
#include <cstdio>
#include <locale>
#include "GameLib/GameLib.h"
#include "GameLib/Base/MemoryManager.h"

//#define STRONG_DEBUG //作ってる人用デバグ

#ifndef NDEBUG //デバグ時だけデバグ情報
#define USE_DEBUG_INFO
#endif

//このファイルの中はC++のライブラリを使わないで書かれている。
//何故かと言えば、newから呼ばれる場所なのに、中でnewするようなことをしたら無限再帰に陥るからだ。
//C++のライブラリは中で勝手にnewするのでどれを使っていいとは判断しがたいのである。
//C言語の関数は基本的に使うなと書いたが、どうしても使うべき時というのはあるものだ。

namespace GameLib{

namespace{ //ある程度安定動作するまではずしておく。デバグしにくくて仕方ない。

using namespace std;

//型別名。コードを短くしたい。
typedef unsigned U4; //バイト無符号別名
typedef unsigned short U2; //2バイト無符号別名

//設定定数群
const U4 MAX_NORMAL_BLOCK_SIZE = 128 * 1024; //これ以上はVirtualAllocで直接確保
const U4 HEAP_REGION_SIZE_BIT = 24; //OSから一気にせしめる領域サイズ(例えば20で1MB, 24で16MB)
const U4 ALIGN = 8; //アラインサイズ4,8,16あたりから選ぼう。4を選ぶ時はdoubleがないことを保障すること。
//導出定数群
const U4 HEAP_REGION_SIZE = ( 1 << HEAP_REGION_SIZE_BIT );
const U4 TABLE_NUMBER = ( HEAP_REGION_SIZE_BIT - 3 ) * 4; //空きリスト箱の数(4ごとに倍になるのでサイズビット数の4倍だが、4,8,12,16と始まるため、いろいろあって-3)

//ブロックフラグ
/*
でかいブロックかを知りたいのは、解放時。解放するときだから、使用中に決まっている。
一方、空かどうかを知りたいのはHeap内とわかってからだから、同じビットを使いまわす。
*/
const U4 FLAG_LARGE_BLOCK = ( 1 << 0 ); //でかいブロックです。
const U4 FLAG_EMPTY = ( 1 << 0 ); //空いてます。
const U4 FLAG_PREV_EMPTY = ( 1 << 1 ); //前のブロックは空きです
const U4 SIZE_MASK = ~( FLAG_PREV_EMPTY | FLAG_EMPTY );

#ifdef USE_DEBUG_INFO
//ファイル名ハッシュ
/*
デバグ情報用のファイル名は、グローバル変数にまとめて置いておく。4バイトを惜しんだためだ。
*/
const U2 FILE_INDEX_MAX = 65521; //65536以下最大の素数
const U2 FILE_INDEX_UNKNOWN = FILE_INDEX_MAX - 1;
const char* gFileNames[ FILE_INDEX_MAX ];

U2 getFileNameIndex( const char* p ){
	if ( !p ){
		return FILE_INDEX_UNKNOWN;
	}
	//ハッシュ値計算
	ptrdiff_t address = p - static_cast< const char* >( 0 );
	U2 h = static_cast< U2 >( address % FILE_INDEX_MAX );
	//ハッシュ値を添え字の初期値としてインデクスゲット
	for ( U2 i = h; i < FILE_INDEX_UNKNOWN; ++i ){
		if ( !gFileNames[ i ] || ( gFileNames[ i ] == p ) ){
			gFileNames[ i ] = p;
			return i;
		}
	}
	//こっちに来るようだと相当終わってる。たぶん満足な速度では動かない。
	for ( U2 i = 0; i < h; ++i ){
		if ( !gFileNames[ i ] || ( gFileNames[ i ] == p ) ){
			gFileNames[ i ] = p;
			return i;
		}
	}
	STRONG_ASSERT( false && "MemoryManager : FileName Table Full!" );
	return FILE_INDEX_UNKNOWN;
}
#endif //DEBUG_INFO

//char*ゲット
template< class T > inline char* ptr( T* p ){
	return reinterpret_cast< char* >( p );
}

//任意の型にキャストして返す。バイト単位オフセット機能付き
template< class T > inline T* cast( void* p, int offsetInByte ){
	return reinterpret_cast< T* >( ptr( p ) + offsetInByte );
}

//アドレス差ゲット
template< class A, class B > inline U4 diff( A* p0, B* p1 ){
	ptrdiff_t t = ptr( p0 ) - ptr( p1 );
	return static_cast< U4 >( t );
}

//ポインタをアライン
template< class T > inline T* align( T* p, U4 n ){
	ptrdiff_t n2 = static_cast<ptrdiff_t>(n - 1);
	ptrdiff_t address = ptr( p ) - static_cast< char* >( 0 );
	address += n2;
	address &= ~( n2 );
	return reinterpret_cast< T* >( address );
}

//整数をアライン
inline U4 align( U4 size, U4 n ){
	size += n - 1;
	size &= ~( n - 1 );
	return size;
}

#ifdef _WIN32
//OSからアラインされたメモリをもらう
void* allocateAlignedMemory( void** originalPointer, U4 size, U4 alignSize ){
	//2倍アドレスを予約
	void* reserved = VirtualAlloc( NULL, size*2, MEM_RESERVE, PAGE_NOACCESS );
	STRONG_ASSERT( reserved && "MemoryManager : Address Space Full!" );
	//適切なアラインアドレスを取得して、そこから実領域確保
	void* aligned = align( reserved, alignSize );
	void* commited = VirtualAlloc( aligned, size, MEM_COMMIT, PAGE_READWRITE );
	STRONG_ASSERT( commited && "MemoryManager : No Memory!" );
	STRONG_ASSERT( ( commited == aligned ) && "MemoryManager : Unexpected Error! VirtualAlloc() returned invalid value." );

	*originalPointer = reserved; //free用アドレスを返す
	return commited;
}

//OSからメモリをもらう。アライン不要
void* allocateMemory( U4 size ){
	U4 alignedSize = align( size, 64 * 1024 );
	void* p = VirtualAlloc( NULL, alignedSize , MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	STRONG_ASSERT( p && "MemoryManager : No Memory!" );
#ifndef NDEBUG //埋め
	memset( p, 0xfb, size ); //未使用領域マーキング
	if ( size < alignedSize ){ //禁止領域マーキング
		memset( ptr( p ) + size, 0xf9, alignedSize - size );
	}
#endif
	return p;
}

//OSにメモリを返す
void deallocateMemory( void* p ){
	BOOL succeeded = VirtualFree( p, 0, MEM_RELEASE );
	STRONG_ASSERT( ( succeeded != 0 ) && "MemoryManager : Deallocation failed! The pointer must be invalid." );
}
#endif

struct Lock{
	Lock(){} //デフォルトコンストラクタは何もしない。グローバルに置かれる事態を想定する。
	Lock( bool f ){
		mLock = ( f ) ? 1 : 0;
	}
	void lock(){
		//何回か試してダメなら一旦スレッドを切り替える。
		int count = 0;
		while ( InterlockedCompareExchange( &mLock, 1, 0 ) != 0 ){
			++count;
			if ( count == 10 ){ //10回くらい？
				count = 0;
				Sleep( 0 );
			}
		}
	}
	bool tryLock(){
		return ( InterlockedCompareExchange( &mLock, 1, 0 ) == 0 );
	}
	void unlock(){
		InterlockedCompareExchange( &mLock, 0, mLock );
	}
	LONG mLock;
};

U4 getBitNumber( U4 a ){
	//二分検索でビット数を確定
	U4 first = 0;
	U4 last = HEAP_REGION_SIZE_BIT;
	while ( last >= first + 2 ){ //差が2以上あれば繰り返す
		U4 middle = ( first + last ) / 2;
		U4 middleV = ( 1 << middle ) - 1;
		if ( middleV < a ){
			first = middle;
		}else if ( middleV > a ){
			last = middle;
		}else{
			first = last = middle;
		}
	}
	//差が0か1になっているので、lastを返す。
	return last;
}

inline U4 getHeadIndex( U4 size ){
	U4 r;
	//テーブルは
	//最初の8個は4i。(0,4,8...)。切捨て。
	//そこから上は32,40,48,56のように4つごとに倍。
	if ( size <= 32 ){	//32バイトまでは線形。4,8,12...と並ぶ
		r = size >> 2;
	}else{
		//まずsizeが何ビットの数なのかを確定させる。
		U4 bits = getBitNumber( size );
		//4bitの数なら、[2bit*5, 2bit*8)の範囲にある。
		//8-9,10-11,12-13,14-15のどの範囲かは、12ビット目、つまり2と4の位を見ればいい。
		//4bitの数なら1bit右シフトして0-7にし、この下位2bitを見る
		U4 idx = ( size >> ( bits - 3 ) ) & 0x3;
		r = ( bits - 4 ) * 4 + idx;
	}
	return r;
}

//[Lb]はLargeBlockの略
class Heap{
public:
	Heap( void* reserved ) :
	mReserved( reserved ),
	mNext( 0 ),
	mLock( false ){
		//最初のでかいブロックを作る。-4はprevSize分ケチってる。最初のブロックより前なんてないからだ。
		U4 body = align( sizeof( Heap ) + VACANT_HEADER_SIZE - 4, ALIGN ) - VACANT_HEADER_SIZE;
		U4 bodySize = HEAP_REGION_SIZE - body - 8; //8バイト減らすのは、次のブロックに無理やり書き込む4バイトと、フラグをセットする次の4倍バイトの分。

		//リンク初期化
		for ( int i = 0; i < TABLE_NUMBER; ++i ){
			U4 headAddr = diff( mHeads[ i ], this );
			U4* head = mHeads[ i ];
			head[ PREV_SIZE ] = head[ SIZE ] = 0xffffffff; //誰も見ないところなので死にそうな数を入れておく。
			head[ NEXT ] = head[ PREV ] = headAddr; //ループ
		}
		//最初のバッファをリストに挿入
		insertToList( body, bodySize );
		setEmptySize( body, bodySize );
		U4 next = body + bodySize;
		setPrevEmptyFlag( next, bodySize );
#ifdef STRONG_DEBUG
check();
#endif
	}
	static Heap* create(){
		void* reserved;
		Heap* r = reinterpret_cast< Heap* >( allocateAlignedMemory( &reserved, HEAP_REGION_SIZE, HEAP_REGION_SIZE ) );

		//自分のメンバを埋める
		new( r ) Heap( reserved );
		return r;
	}
	void removeFromList( U4 addr ){
		if ( getSize( addr ) >= VACANT_HEADER_SIZE ){ //サイズが足りてないものは遅延開放中。
			U4 next = getNext( addr );
			U4 prev = getPrev( addr );
			setPrev( next, prev );
			setNext( prev, next );
		}
	}
	void insertToList( U4 addr, U4 size ){
		STRONG_ASSERT( addr < 0xfffff8 );
		U4 index = getHeadIndex( size - OCCUPYED_HEADER_SIZE + 4 );//ヘッダ分引いて、4バイトのブロックに食い込むので、このサイズが正味。
		//新しいリストに差し込む。先頭に挿入
		U4 first = mHeads[ index ][ NEXT ];
		U4 last = getPrev( first );

		setNext( last, addr );
		setPrev( addr, last );
		setNext( addr, first );
		setPrev( first, addr );
	}

#ifdef USE_DEBUG_INFO
	void* allocate( U4 size, U4 debugInfo ){
#else
	void* allocate( U4 size ){
#endif
#ifdef STRONG_DEBUG
check();
#endif
		void* r = 0;
		U4 headIndex = getHeadIndex( size ) + 1; //このサイズが十分に入る箱が欲しいので、+1する必要がある
		for ( U4 i = headIndex; i < TABLE_NUMBER; ++i ){
			U4 head = diff( mHeads[ i ], this );
			U4 current = mHeads[ i ][ NEXT ];
			if ( current != head ){
				U4 currentSizeWithFlag = getSize( current );
				U4 currentSize = currentSizeWithFlag & SIZE_MASK;
				U4 next = current + currentSize;
				//8バイトアラインした戻りアドレスを計算(+4は次のヘッダの最初4バイトを破壊する気だから)
				U4 user = ( next + 4 - size ) & ~( ALIGN - 1 );
				//そこから使用中ヘッダサイズを引いたのがブロック先頭アドレス
				U4 newBlock = user - OCCUPYED_HEADER_SIZE;
				//このブロックのヘッダ分以上のこるのであれば、分割
				if ( newBlock >= current + VACANT_HEADER_SIZE ){
					//まずリストから外す
					removeFromList( current );
					//自分のサイズを再設定
					currentSize = newBlock - current;
					setEmptySize( current, currentSize );
					//再設定したサイズでリストに再登録
					insertToList( current, currentSize );
					//新ブロック設定
					setSize( newBlock, next - newBlock );
#ifdef USE_DEBUG_INFO
					setDebugInfo( newBlock, debugInfo );
#endif
					//フラグ設定
					setPrevEmptyFlag( newBlock, currentSize ); //前は絶対空いてる
					resetPrevEmptyFlag( next ); //その次は使ってる
#ifdef STRONG_DEBUG
check();
#endif
				}else{ //残らない。このブロックをそのまま用いる。
					removeFromList( current ); //リストから切り離す
#ifdef USE_DEBUG_INFO
					setDebugInfo( current, debugInfo );
#endif
					//フラグ設定
					resetEmptyFlag( current ); //空いてない
					resetPrevEmptyFlag( next ); //空いてないことを伝える
					//返却アドレス
					user = current + OCCUPYED_HEADER_SIZE;
#ifdef STRONG_DEBUG
check();
#endif
				}
				r = ptr( this ) + user;
#ifndef NDEBUG //デバグ用塗りつぶし
				memset( r, 0xfb, size ); //未使用はfb
				char* paddingBegin = ptr( r ) + size;
				U4 paddingSize = next + 4 - user - size; //+4ははみだす分
				memset( paddingBegin, 0xf9, paddingSize ); //使用禁止領域はf9
#endif
				break; //確保終了
			}
		}
		return r;
	}
	void deallocate( void* p ){
#ifdef STRONG_DEBUG
check();
#endif
		U4 addr = diff( p, this ) - OCCUPYED_HEADER_SIZE;
		U4 sizeWithFlag = getSize( addr );
		//削除は、まず前後との融合が可能かどうかを調べる。
		bool prevIsEmpty = ( ( sizeWithFlag & FLAG_PREV_EMPTY ) != 0 );
		U4 size = ( sizeWithFlag & SIZE_MASK );
		U4 next = addr + size;
#ifndef NDEBUG 
		//付近の整合性チェック
		if ( prevIsEmpty ){
			U4 prevSize = getPrevSize( addr );
			U4 prev = addr - prevSize;
			U4 prevSizeWithFlag = getSize( prev );
			STRONG_ASSERT( prevSizeWithFlag & FLAG_EMPTY );
			STRONG_ASSERT( prevSize == ( prevSizeWithFlag & SIZE_MASK ) );
		}
		U4 nextSizeWithFlagDebug = getSize( next );
		STRONG_ASSERT( !( nextSizeWithFlagDebug & FLAG_PREV_EMPTY ) ); //使ってるんだから空いてるわけないです。

		//解放済みマークで塗りつぶし
		ptrdiff_t fillSize = ptr( this ) + next - ptr( p );
		if ( fillSize > 0 ){
			memset( p, 0xfd, fillSize ); //使用済みはfb
		}
#endif
		U4 nextSizeWithFlag = getSize( next );
		bool nextIsEmpty = ( ( nextSizeWithFlag & FLAG_EMPTY ) != 0 );
		U4 nextSize = ( nextSizeWithFlag & SIZE_MASK );

		if ( prevIsEmpty ){ //前が空いてる。
			U4 prevSize = getPrevSize( addr );
			U4 prev = addr - prevSize;
			if ( nextIsEmpty ){ //次も空いてる
				//前後とも一回外して、
				removeFromList( prev );
				removeFromList( next );
				//サイズ再設定
				prevSize += size + nextSize;
				setEmptySize( prev, prevSize );
				insertToList( prev, prevSize );
				//次のブロックに空いていることを伝える
				U4 nextNext = prev + prevSize;
				setPrevEmptyFlag( nextNext, prevSize );
#ifdef STRONG_DEBUG
check();
#endif
			}else{ //前だけ空いてる
				removeFromList( prev );
				//サイズ再設定
				prevSize += size;
				setEmptySize( prev, prevSize );
				insertToList( prev, prevSize );
				//次のブロックに空いていることを伝える
				setPrevEmptyFlag( next, prevSize );
#ifdef STRONG_DEBUG
check();
#endif
			}
		}else{
			if ( nextIsEmpty ){ //次も空いてる
				removeFromList( next );
				//サイズ再設定
				size += nextSize;
				setEmptySize( addr, size );
				insertToList( addr, size );
				//次のブロックに空いているサイズを伝える
				U4 nextNext = addr + size;
				setPrevSize( nextNext, size );
#ifdef STRONG_DEBUG
check();
#endif
			}else{ //くっついてない。
				if ( size >= VACANT_HEADER_SIZE ){ //リストにつなげる状況でだけつなぐ。足りないときは隣が解放されるのを待つ。
					insertToList( addr, size );
				}
				setEmptyFlag( addr );
				//次のブロックに空いていることを伝える
				setPrevEmptyFlag( next, size );
#ifdef STRONG_DEBUG
check();
#endif
			}
		}
	}
	void check(){
		/*
		ここで行うチェックは、
		１．アドレス順のチェック。前サイズ、サイズ、ブロック境界等々の整合性
		２．リンク昇順のチェック。空いているか。サイズは適正か。
		３．リンク降順のチェック。空いているか。サイズは適正か。
		*/
		U4 begin = align( sizeof( Heap ) + VACANT_HEADER_SIZE - 4, ALIGN ) - VACANT_HEADER_SIZE;
		U4 current = begin;
		U4 prevSizeCheck = 0;
		bool prevIsEmpty = false;
		U4 end = HEAP_REGION_SIZE - OCCUPYED_HEADER_SIZE; //ここより前だろ。でないとヘッダがはいらん
		while ( current < end ){
			U4 sizeWithFlag = getSize( current );
			if ( sizeWithFlag & FLAG_PREV_EMPTY ){
				STRONG_ASSERT( prevIsEmpty );
				U4 prevSize = getPrevSize( current );
				STRONG_ASSERT( prevSize == prevSizeCheck );
			}
			prevIsEmpty = ( sizeWithFlag & FLAG_EMPTY ) ? true : false;
			U4 size = sizeWithFlag & SIZE_MASK;
			current += size;
			prevSizeCheck = size;
		}
		for ( U4 i = 0; i < TABLE_NUMBER; ++i ){
			U4 head = diff( mHeads[ i ], this );
			U4 current;
			//リンク昇順チェック
			current = mHeads[ i ][ NEXT ];
			while ( current != head ){
				U4 sizeWithFlag = getSize( current );
				STRONG_ASSERT( sizeWithFlag & FLAG_EMPTY ); //空いてるはずだろ。
				U4 size = sizeWithFlag & SIZE_MASK;
				U4 idx = getHeadIndex( size - OCCUPYED_HEADER_SIZE + 4 );//ヘッダ分引いて、4バイトのブロックに食い込むので、このサイズが正味。
				STRONG_ASSERT( idx == i );
				current = getNext( current );
			}
			//リンク降順チェック
			current = mHeads[ i ][ PREV ];
			while ( current != head ){
				U4 sizeWithFlag = getSize( current );
				STRONG_ASSERT( sizeWithFlag & FLAG_EMPTY ); //空いてるはずだろ。
				U4 size = sizeWithFlag & SIZE_MASK;
				U4 idx = getHeadIndex( size - OCCUPYED_HEADER_SIZE + 4 );//ヘッダ分引いて、4バイトのブロックに食い込むので、このサイズが正味。
				STRONG_ASSERT( idx == i );
				current = getPrev( current );
			}
		}
	}
	void write( FILE* fp ){
		char str[ 8192 ];
		U4 begin = align( sizeof( Heap ) + VACANT_HEADER_SIZE - 4, ALIGN ) - VACANT_HEADER_SIZE;
		U4 current = begin;
		U4 end = HEAP_REGION_SIZE - OCCUPYED_HEADER_SIZE; //ここより前だろ。でないとヘッダがはいらん
		while ( current < end ){
			U4 sizeWithFlag = getSize( current );
			U4 size = sizeWithFlag & SIZE_MASK;
			if ( !( sizeWithFlag & FLAG_EMPTY ) ){
#ifdef USE_DEBUG_INFO
				U4 debugInfo = getDebugInfo( current );
				const char* filename = gFileNames[ debugInfo & 0xffff ];
				U4 line = debugInfo >> 16;
#else
				const char* filename = 0;
				U4 line = 0;
#endif
				if ( !filename ){
					filename = "unknown";
				}
				int l = sprintf_s( 
					str, 
					8192,
					"%p\t\t%d\t\t%s\t\t%d\n",
					ptr( this ) + current,
					size,
					filename,
					line );
				if ( fp ){
					fwrite( str, 1, l, fp );
				}else{
					::OutputDebugStringA( str );
				}
			}
			current += size;
		}
	}
	void lock(){
		mLock.lock();
	}
	bool tryLock(){
		return mLock.tryLock();
	}
	void unlock(){
		mLock.unlock();
	}
	enum Header{
		PREV_SIZE = 0, //こいつは前のブロックが空いてる時だけつかう。それ以外の時は壊されている。
		SIZE = 1,
#ifdef USE_DEBUG_INFO
		DEBUG_INFO = 2, //次と共用。こいつは使用中しか使わない。
#endif
		NEXT = 2,
		PREV = 3,

		HEADER_MAX = 4,
	};
#ifdef USE_DEBUG_INFO
	static const U4 OCCUPYED_HEADER_SIZE = 12;
#else
	static const U4 OCCUPYED_HEADER_SIZE = 8;
#endif
	static const U4 VACANT_HEADER_SIZE = 16;
	//こまごまとしたヘッダ操作関数群
	void setNext( U4 addr, U4 target ){
		( cast< U4 >( this, addr ) )[ NEXT ] = target;
	}
	void setPrev( U4 addr, U4 target ){
		( cast< U4 >( this, addr ) )[ PREV ] = target;
	}
	void setSize( U4 addr, U4 size ){
		( cast< U4 >( this, addr ) )[ SIZE ] = size;
	}
	void setEmptySize( U4 addr, U4 size ){
		( cast< U4 >( this, addr ) )[ SIZE ] = size | FLAG_EMPTY;
	}
	void setPrevSize( U4 addr, U4 size ){
		( cast< U4 >( this, addr ) )[ PREV_SIZE ] = size;
	}
	void setPrevEmptyFlag( U4 addr, U4 prevSize ){
		U4* p4 = cast< U4 >( this, addr );
		p4[ SIZE ] |= FLAG_PREV_EMPTY;
		p4[ PREV_SIZE ] = prevSize;
	}
	void setEmptyFlag( U4 addr ){
		( cast< U4 >( this, addr ) )[ SIZE ] |= FLAG_EMPTY;
	}
	void resetEmptyFlag( U4 addr ){
		( cast< U4 >( this, addr ) )[ SIZE ] &= ~FLAG_EMPTY;
	}
	void resetPrevEmptyFlag( U4 addr ){
		( cast< U4 >( this, addr ) )[ SIZE ] &= ~FLAG_PREV_EMPTY;
	}
	U4 getSize( U4 addr ){
		return ( cast< U4 >( this, addr ) )[ SIZE ];
	}
	U4 getPrevSize( U4 addr ){
		return ( cast< U4 >( this, addr ) )[ PREV_SIZE ];
	}
	U4 getNext( U4 addr ){
		return ( cast< U4 >( this, addr ) )[ NEXT ];
	}
	U4 getPrev( U4 addr ){
		return ( cast< U4 >( this, addr ) )[ PREV ];
	}
#ifdef USE_DEBUG_INFO
	void setDebugInfo( U4 addr, U4 debugInfo ){
		( cast< U4 >( this, addr ) )[ DEBUG_INFO ] = debugInfo;
	}
	U4 getDebugInfo( U4 addr ){
		return ( cast< U4 >( this, addr ) )[ DEBUG_INFO ];
	}
#endif
	void* mReserved; //予約領域ポインタ
	Heap* mNext; //次のヒープ
	U4 mHeads[ TABLE_NUMBER ][ HEADER_MAX ]; //サイズごとの空きリスト
	Lock mLock;
};

class Impl{
public:
#ifdef USE_DEBUG_INFO
	void* allocate( size_t sizeOrig, U4 debugInfo = FILE_INDEX_UNKNOWN ){
#else
	void* allocate( size_t sizeOrig ){
#endif
#ifdef _WIN64
		STRONG_ASSERT( sizeOrig <= 0xffffffff && "allocation over 4GB is forbidden" );
#endif
		U4 size = static_cast< U4 >( sizeOrig ); //4GB以上の
		//戻り値
		void* r = 0;
		//sizeが限界を超えている。専用ブロックを用意
		if ( size > MAX_NORMAL_BLOCK_SIZE ){
#ifdef USE_DEBUG_INFO
			r = allocateLb( size, debugInfo );
#else
			r = allocateLb( size );
#endif
		}else{
			//ヒープにallocate()させる。ダメなら次へ。
			Heap* current = mHead; //ロック不要。0なら0で結構。0以外ならどれでもかまわん。
			while ( current ){
				if ( current->tryLock() ){
#ifdef USE_DEBUG_INFO
					r = current->allocate( size, debugInfo );
#else
					r = current->allocate( size );
#endif
					current->unlock();
				}
				if ( r ){
					break;
				}
				current = current->mNext;
			}

			if ( !r ){
				//空きがない。ヒープを足さないといけません。
				Heap* newHeap = Heap::create();
#ifdef USE_DEBUG_INFO
				r = newHeap->allocate( size, debugInfo );
#else
				r = newHeap->allocate( size );
#endif
				//先頭に足します
				mHeapLock.lock();
				Heap* first = mHead;
				mHead = newHeap;
				newHeap->mNext = first;
				mTotalSize += HEAP_REGION_SIZE;
				mHeapLock.unlock();
			}
		}
STRONG_ASSERT( r && "BUG!" ); //ここはありえない。バグだ
		return r;
	}
	void deallocate( void* p ){
		if ( !p ){ //0回避
			return;
		}
		//mNextが0なら、でかいブロックです。
		if ( isLb( p ) ){
			deallocateLb( p );
		}else{ //そうでない場合、まずAreaの先頭を割り出して、Areaに削除させる
			Heap* heap = getHeap( p );
			heap->lock();
			heap->deallocate( p );
			heap->unlock();
		}
	}
	void write( const char* filename ){
		//この中でnewを使うわけには行かない。
		//それは即座にofstreamもだめだということを意味する。中でnewするからだ。
		//よって、このファイルの中ではC標準ライブラリで事を行う。
		FILE* fp = 0;
		if ( filename ){
			setlocale( LC_ALL, "" );
			errno_t e = fopen_s( &fp, filename, "w" );
			STRONG_ASSERT( ( e == 0 ) && "MemoryManager::write() : can't open output file." );
		}else{
			::OutputDebugStringA( "\n" );
		}
		char str[ 8192 ]; //大過剰に取るよ。ここでケチっても何の得もないからな。本当sprintfはダメだなと思う。
		int l = 0;
		l += sprintf_s( str + l, 8192 - l, "[ MemoryManager::write() ]\n" );
		l += sprintf_s( str + l, 8192 - l, "totalSize = %d ( %x ) : %.2fMB\n", mTotalSize, mTotalSize, static_cast< float >( mTotalSize ) / 1024.f / 1024.f );
		l += sprintf_s( str + l, 8192 - l, "address\t\tsize\t\tfile\t\tline\n" );
		if ( fp ){
			fwrite( str, 1, l, fp );
		}else{
			::OutputDebugStringA( str );
		}

		mLbLock.lock();
		//まず大ブロックから
		if ( mLbHead ){
			void* begin = mLbHead;
			void* block = begin;
			do{
#ifdef USE_DEBUG_INFO
				U4 debugInfo = getLbDebugInfo( block );
				const char* filename = gFileNames[ debugInfo & 0xffff ];
				U4 line = debugInfo >> 16;
#else
				const char* filename = 0;
				U4 line = 0;
#endif
				if ( !filename ){
					filename = "unknown";
				}
				U4 size = getLbSize( block );
				l = sprintf_s( 
					str, 
					8192,
					"%p\t\t%d\t\t%s\t\t%d\n",
					block,
					size,
					filename,
					line );
				if ( fp ){
					fwrite( str, 1, l, fp );
				}else{
					::OutputDebugStringA( str );
				}
				block = getLbNext( block );
			} while ( begin != block );
		}
		mLbLock.unlock();
		//次ヒープ
		mHeapLock.lock();
		Heap* current = mHead;
		while ( current ){
			current->write( fp );
			current = current->mNext;
		}
		mHeapLock.unlock();
	}
	void check(){
		checkLb();
		checkHeap();
	}
	~Impl(){
#ifndef NDEBUG
		write( 0 );
#endif
	}
	int totalSize() const {
		return mTotalSize;
	}
private:
	void checkLb(){
		mLbLock.lock();
		//まず大ブロックから
		if ( mLbHead ){
			void* begin = mLbHead;
			void* block = begin;
			do{
				void* next = getLbNext( block );
				void* prev = getLbPrev( block );
				void* nextPrev = getLbPrev( next );
				void* prevNext = getLbNext( prev );
				STRONG_ASSERT( nextPrev == block );
				STRONG_ASSERT( prevNext == block );
				block = next;
			} while ( begin != block );
		}
		mLbLock.unlock();
	}
	void checkHeap(){
		Heap* h = mHead;
		while ( h ){
			h->lock();
			h->check();
			h->unlock();
		}
	}
	Heap* getHeap( void* p ){
		ptrdiff_t address = ptr( p ) - static_cast< char* >( 0 );
		address &= ~( static_cast<ptrdiff_t>(HEAP_REGION_SIZE - 1) );
		return reinterpret_cast< Heap* >( address );
	}
#ifdef USE_DEBUG_INFO
	void* allocateLb( U4 size, U4 debugInfo ){
#else
	void* allocateLb( U4 size ){
#endif
#ifdef STRONG_DEBUG
checkLb();
#endif
		//サイズの下位2ビットを0にするためにアライン
		size = align( size, 8 );
		//ヘッダサイズ計算。ポインタ三つとU4二つ
		U4 alignedHeaderSize = align( LB_HEADER_SIZE, ALIGN );
		void* p = allocateMemory( size + alignedHeaderSize );

		//ユーザに返すポインタ
		void* r = ptr( p ) + alignedHeaderSize; 
		setLbSize( r, size );
#ifdef USE_DEBUG_INFO
		setLbDebugInfo( r, debugInfo );
#endif

		//リンク差し替え
		mLbLock.lock();
		void* prev;
		void* next;
		if ( mLbHead ){
			prev = mLbHead;
			next = getLbNext( mLbHead );
			setLbPrev( next, r );
			setLbNext( mLbHead, r );
		}else{
			prev = next = r;
		}
		setLbNext( r, next );
		setLbPrev( r, prev );
		mLbHead = r;
		mTotalSize += size;
		mLbLock.unlock();
#ifdef STRONG_DEBUG
checkLb();
#endif
		return r;
	}
	void deallocateLb( void* p ){
#ifdef STRONG_DEBUG
checkLb();
#endif

		//リンク解除
		mLbLock.lock();
		void* prev = getLbPrev( p );
		void* next = getLbNext( p );
#ifndef NDEBUG
		void* nextPrev = getLbPrev( next );
		void* prevNext = getLbNext( prev );
		STRONG_ASSERT( nextPrev == p );
		STRONG_ASSERT( prevNext == p );
#endif
		setLbNext( prev, next );
		setLbPrev( next, prev );
		if ( mLbHead == p ){ //先頭の削除が来た。
			mLbHead = next; //次のポインタに変更
			if ( mLbHead == p ){ //次にしてもなお同じということは最後の一個だ。
				mLbHead = 0; //抹殺
			}
		}
		mTotalSize -= getLbSize( p ) & SIZE_MASK;
		mLbLock.unlock();
#ifdef STRONG_DEBUG
checkLb();
#endif
		//メモリ解放
		char* origP = ptr( p ) - LB_HEADER_SIZE;
		U4 alignedHeaderSize = align( LB_HEADER_SIZE, ALIGN );
		origP -= alignedHeaderSize - LB_HEADER_SIZE; //アラインでずらしたなら、返却アドレスもずらす
		deallocateMemory( origP );
	}
/* //大ブロックのレイアウト
	void* next;
	void* prev;
	U4 size;
	U4 debugInfo; //この直後をユーザに返す
*/
#ifdef USE_DEBUG_INFO
	static const int DEBUG_INFO = sizeof( U4 );
	static const int SIZE = DEBUG_INFO + sizeof( U4 );
#else
	static const int SIZE = sizeof( U4 );
#endif
	static const int PREV = SIZE + sizeof( void* );
	static const int NEXT = PREV + sizeof( void* );
	static const int LB_HEADER_SIZE = NEXT;

	static bool isLb( void* p ){//サイズは必ず4バイト前にある。
		U4* up = cast< U4 >( p, -SIZE );
		return ( *up & FLAG_LARGE_BLOCK );
	}
#ifdef USE_DEBUG_INFO
	static U4 getLbDebugInfo( void* p ){
		return *cast< U4 >( p, -DEBUG_INFO );
	}
	static void setLbDebugInfo( void* p, U4 di ){
		*cast< U4 >( p, -DEBUG_INFO ) = di;
	}
#endif
	static U4 getLbSize( void* p ){
		return *cast< U4 >( p, -SIZE );
	}
	static void* getLbPrev( void* p ){
		return *cast< void* >( p, -PREV );
	}
	static void* getLbNext( void* p ){
		return *cast< void* >( p, -NEXT );
	}
	static void setLbSize( void* p, U4 size ){
		*cast< U4 >( p, -SIZE ) = size | FLAG_LARGE_BLOCK;
	}
	static void setLbPrev( void* p, void* prev ){
		*cast< void* >( p, -PREV ) = prev;
	}
	static void setLbNext( void* p, void* next ){
		*cast< void* >( p, -NEXT ) = next;
	}

	//ヒープ先頭
	Heap* mHead;
	//大ブロック先頭
	void* mLbHead;
	//ロック
	Lock mLbLock;
	Lock mHeapLock;
	//全サイズ
	int mTotalSize;
};
Impl gImpl;

} //namespace {}

MemoryManager MemoryManager::instance(){
	return MemoryManager();
}

void MemoryManager::write( const char* filename ){
	gImpl.write( filename );
}

int MemoryManager::totalSize() const {
	return gImpl.totalSize();
}

} //naemsapce GameLib

using namespace GameLib;

#ifdef USE_DEBUG_INFO
void* operator new( size_t size, const char* filename, int line ){
	unsigned debugInfo = ( line << 16 ) | getFileNameIndex( filename );
	return gImpl.allocate( size, debugInfo );
#else
void* operator new( size_t size, const char*, int ){
	return gImpl.allocate( size );
#endif
}

#ifdef USE_DEBUG_INFO
void* operator new[]( size_t size, const char* filename, int line ){
	unsigned debugInfo = ( line << 16 ) | getFileNameIndex( filename );
	return gImpl.allocate( size, debugInfo );
#else
void* operator new[]( size_t size, const char*, int ){
	return gImpl.allocate( size );
#endif
}

void operator delete( void* p, const char*, int ){
	gImpl.deallocate( p );
}

void operator delete[]( void* p, const char*, int ){
	gImpl.deallocate( p );
}

void* operator new( size_t size ){
	return gImpl.allocate( size );
}

void* operator new[]( size_t size ){
	return gImpl.allocate( size );
}

void operator delete( void* p ){
	gImpl.deallocate( p );
}

void operator delete[]( void* p ){
	gImpl.deallocate( p );
}
