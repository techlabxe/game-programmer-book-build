#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

//わかりやすいように値はdoubleにしてある。intは全部場所を指す添え字。
class List{
public:
	List( int capacity );
	~List();
	//追加 全て場所を返す
	int addBefore( int position, double value ); //指定場所の前に追加
	int addAfter( int position, double value ); //指定場所の後に追加
	int addHead( double value ); //先頭追加
	int addTail( double value ); //末尾追加
	//削除
	void remove( int position ); //指定場所を削除
	void removeHead(); //先頭を削除
	void removeTail(); //末尾を削除
	//取得系
	double value( int position ) const;
	int next( int position ) const;
	int previous( int position ) const;
	int first() const;
	int last() const;
private:
	//3つ配列を持つのも面倒なので構造体に
	struct Node{
		int mNext;
		int mPrev;
		double mValue;
	};
	Node* mNodes;
	int mAddPosition;
	int mCapacity;
};

#endif

