#ifndef INCLUDED_GAMELIB_LIST_H
#define INCLUDED_GAMELIB_LIST_H

class List{
public:
	explicit List( int capacity );
	~List();
	//末尾追加。どこに足したかを返す。
	int addTail( int value );
	//指定場所を削除
	void remove( int position );
	///値取得
	int value( int position ) const;
	//末端かどうかはisEnd()で確認せよ
	int next( int position ) const;
	//先頭取得
	int first() const;
	//positionが末端か判定。
	bool isEnd( int position ) const;
private:
	struct Node{
		int mNext;
		int mPrev;
		int mValue;
	};
	List( const List& ); //コピーコンストラクタ禁止
	void operator=( const List& ); //代入禁止

	Node* mNodes;
	int* mEmptyStack; //これでどこが空いているかがO(1)でわかる
	int mStackTop;
	const int mCapacity; //コンストラクトしたらいじらないのでconst
};

//cppを作るのが面倒なのでここに書いてしまう。速度も上がるしいいだろう。

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
//mNodes[ mCapacity ]はダミーノードだ。先頭と末尾にこのダミーがあるおかげでいろいろ楽なのである。
inline List::List( int capacity ) : 
mNodes( 0 ),
mEmptyStack( 0 ),
mCapacity( capacity ){
	mNodes = new Node[ mCapacity + 1 ]; //最後にダミーノード。だから+1
	mEmptyStack = new int[ mCapacity ];
	for ( int i = 0; i < mCapacity; ++i ){
		mEmptyStack[ i ] = i; //空いてる番号を格納。最初だから全部な。
	}
	mStackTop = mCapacity - 1; //最初に取り出すのは末尾
	//ダミーノード設定。
	mNodes[ mCapacity ].mNext = mNodes[ mCapacity ].mPrev = mCapacity;
}

inline List::~List(){
	SAFE_DELETE_ARRAY( mNodes );
	SAFE_DELETE_ARRAY( mEmptyStack );
}

inline int List::addTail( int v ){
	//空き場所を取得
	int newPos = mEmptyStack[ mStackTop ];
	--mStackTop;

	mNodes[ newPos ].mValue = v; //値を入れて、
	mNodes[ newPos ].mNext = mCapacity; //次はダミーノード
	int oldTail = mNodes[ mCapacity ].mPrev; //前の末尾はダミーの前にいた
	mNodes[ newPos ].mPrev = oldTail; //新しい末尾の前に古い末尾を入れる

	mNodes[ oldTail ].mNext = newPos; //古い末尾の次が自分
	mNodes[ mCapacity ].mPrev = newPos; //ダミーの前が自分
	return newPos; //足した場所を返す
}

inline void List::remove( int position ){
	Node* cur = &mNodes[ position ]; //自分
	Node* nxt = &mNodes[ cur->mNext ]; //次
	Node* prev = &mNodes[ cur->mPrev ]; //前
	//前の奴の次を、次にする。
	prev->mNext = cur->mNext;
	//次の奴の前を、前にする。
	nxt->mPrev = cur->mPrev;

	//空き番号リストに追加
	++mStackTop;
	mEmptyStack[ mStackTop ] = position;
}

inline int List::value( int position ) const {
	return mNodes[ position ].mValue;
}

inline int List::next( int position ) const {
	return mNodes[ position ].mNext;
}

inline int List::first() const {
	//ダミーの次が先頭なのだよ
	return mNodes[ mCapacity ].mNext;
}

inline bool List::isEnd( int position ) const {
	//最後まで行けばmCapacity、つまりダミーになる。
	return ( position == mCapacity );
}

#endif

