#include "List.h"

//ダミー要素を使うことでコードがとっても簡単に！
static const int DUMMY_HEAD = 0;
static const int DUMMY_TAIL = 1;

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
List::List( int capacity ) : 
mNodes( 0 ),
mAddPosition( 0 ),
mCapacity( capacity + 2 ){
	mNodes = new Node[ capacity + 2 ];
	//0番は特別。最初の要素の前にあるダミー要素。
	Node* e;
	e = &mNodes[ DUMMY_HEAD ];
	e->mNext = 1;
	e->mPrev = -1;
	e->mValue = 0.0;
	//1番も特別。最後の要素のさらに後にあるダミー要素。
	e = &mNodes[ DUMMY_TAIL ];
	e->mNext = -1;
	e->mPrev = 0;
	e->mValue = 0.0;

	mAddPosition = 2; //2番目から追加開始
}

List::~List(){
	delete[] mNodes;
	mNodes = 0;
}

int List::addAfter( int position, double v ){
	//足す場所はaddPosition
	Node* e = &mNodes[ mAddPosition ];
	//指定の奴を取り出す
	Node* cur = &mNodes[ position ];
	//次の奴を取り出す
	Node* next = &mNodes[ cur->mNext ];
	//値格納
	e->mValue = v;
	e->mPrev = position; //指定場所が前に
	e->mNext = cur->mNext; //指定の次のが次に
	//指定の奴は前になるので、次は足した奴になる
	cur->mNext = mAddPosition;
	//指定の次の奴は後になるので、前は足した奴になる
	next->mPrev = mAddPosition;
	++mAddPosition;

	return mAddPosition - 1;
}

int List::addBefore( int position, double v ){
	//足す場所はaddPosition
	Node* e = &mNodes[ mAddPosition ];
	//指定の奴を取り出す
	Node* cur = &mNodes[ position ];
	//前の奴を取り出す
	Node* prev = &mNodes[ cur->mPrev ];
	//値格納
	e->mValue = v;
	e->mPrev = cur->mPrev; //指定の前の奴が前
	e->mNext = position; //指定の奴が次
	//指定の奴は後になるので、前は足した奴になる
	cur->mPrev = mAddPosition;
	//指定の前の奴は前になるので、後は足した奴になる
	prev->mNext = mAddPosition;
	++mAddPosition;

	return mAddPosition - 1;
}

int List::addHead( double v ){
	return addBefore( first(), v ); //ダミー要素のおかげでこう書ける。
}

int List::addTail( double v ){
	return addAfter( last(), v ); //ダミー要素のおかげでこう書ける
}

void List::remove( int position ){
	//指定の奴
	Node* cur = &mNodes[ position ];
	//次
	Node* next = &mNodes[ cur->mNext ];
	//前
	Node* prev = &mNodes[ cur->mPrev ];
	//前の奴の次を、次にする。
	prev->mNext = cur->mNext;
	//次の奴の前を、前にする。
	next->mPrev = cur->mPrev;

	//おまけ。最後を消したようならaddPositionを巻き戻してやる。
	//少しは多く足せるようになるかもな。
	if ( position == mAddPosition - 1 ){
		--mAddPosition;
	}
}

void List::removeHead(){ //これはほとんど別名。
	remove( first() );
}

void List::removeTail(){ //これはほとんど別名。
	remove( last() );
}

double List::value( int position ) const {
	return mNodes[ position ].mValue;
}

int List::next( int position ) const {
	int r = mNodes[ position ].mNext;
	return ( r >= 2 ) ? r : -1; //ダミーを指してれば-1に置換
}

int List::previous( int position ) const {
	int r = mNodes[ position ].mPrev;
	return ( r >= 2 ) ? r : -1; //ダミーを指してれば-1に置換
}

int List::first() const {
	return mNodes[ DUMMY_HEAD ].mNext; //0番目はダミー先頭なので、その次が本当の先頭
}

int List::last() const {
	return mNodes[ DUMMY_TAIL ].mPrev; //1番目はダミー末尾なので、その前が本当の末尾
}
