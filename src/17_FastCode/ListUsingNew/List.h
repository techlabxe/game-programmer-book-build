#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

//わかりやすいように値はdoubleにしてある。
class List{
public:
	struct Node{
		Node* mNext;
		Node* mPrev;
		double mValue;
	};
	List();
	~List();
	//追加 全て場所を返す
	Node* addBefore( Node* , double value ); //指定場所の前に追加
	Node* addAfter( Node*, double value ); //指定場所の後に追加
	Node* addHead( double value ); //先頭追加
	Node* addTail( double value ); //末尾追加
	//削除
	void remove( Node* ); //指定場所を削除
	void removeHead(); //先頭を削除
	void removeTail(); //末尾を削除
	//取得系
	double value( Node* ) const;
	Node* next( Node* ) const;
	Node* previous( Node* ) const;
	Node* first() const;
	Node* last() const;
private:
	Node mDummyHead;
	Node mDummyTail;
};

#endif

