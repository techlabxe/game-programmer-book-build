#include "List.h"

//先頭と末尾が特別扱いされないようにちょっと工夫をしている。
List::List(){
	//最初よりさらに前にあるダミー要素
	mDummyHead.mNext = &mDummyTail;
	mDummyHead.mPrev = 0;
	mDummyHead.mValue = 0.0;
	//最後の要素のさらに後にあるダミー要素。
	mDummyTail.mNext = 0;
	mDummyTail.mPrev = &mDummyHead;
	mDummyTail.mValue = 0.0;
}

List::~List(){
	//全部消して回らないといけない
	Node* p = first();
	while ( p ){
		Node* nxt = next( p );
		delete p;
		p = nxt;
	}
}

List::Node* List::addAfter( Node* cur, double v ){
	Node* e = new Node();
	//次の奴を取り出す
	Node* nxt = cur->mNext;
	//値格納
	e->mValue = v;
	e->mPrev = cur; //指定場所が前に
	e->mNext = nxt; //指定の次のが次に
	//指定の奴は前になるので、次は足した奴になる
	cur->mNext = e;
	//指定の次の奴は後になるので、前は足した奴になる
	nxt->mPrev = e;

	return e;
}

List::Node* List::addBefore( Node* cur, double v ){
	Node* e = new Node();
	//前の奴を取り出す
	Node* prev = cur->mPrev;
	//値格納
	e->mValue = v;
	e->mPrev = prev; //指定の前の奴が前
	e->mNext = cur; //指定の奴が次
	//指定の奴は後になるので、前は足した奴になる
	cur->mPrev = e;
	//指定の前の奴は前になるので、後は足した奴になる
	prev->mNext = e;

	return e;
}

List::Node* List::addHead( double v ){
	return addBefore( first(), v ); //ダミー要素のおかげでこう書ける。
}

List::Node* List::addTail( double v ){
	return addAfter( last(), v ); //ダミー要素のおかげでこう書ける
}

void List::remove( Node* cur ){
	//次
	Node* nxt = cur->mNext;
	//前
	Node* prev = cur->mPrev;
	//前の奴の次を、次にする。
	prev->mNext = nxt;
	//次の奴の前を、前にする。
	nxt->mPrev = prev;
	//消す
	delete cur;
	cur = 0;
}

void List::removeHead(){ //これはほとんど別名。
	remove( first() );
}

void List::removeTail(){ //これはほとんど別名。
	remove( last() );
}

double List::value( Node* p ) const {
	return p->mValue;
}

List::Node* List::next( Node* p ) const {
	Node* r = p->mNext;
	if ( r == &mDummyHead || r == &mDummyTail ){
		return 0;
	}else{
		return r;
	}
}

List::Node* List::previous( Node* p ) const {
	Node* r = p->mPrev;
	if ( r == &mDummyHead || r == &mDummyTail ){
		return 0;
	}else{
		return r;
	}
}

List::Node* List::first() const {
	return mDummyHead.mNext; //ダミー先頭の次が本当の先頭
}

List::Node* List::last() const {
	return mDummyTail.mPrev; //ダミー末尾の前が本当の末尾
}
