#ifndef INCLUDED_GAMELIB_COLLISIONMESH_H
#define INCLUDED_GAMELIB_COLLISIONMESH_H

namespace GameLib{
namespace PseudoXml{
	class ConstElement;
}
namespace Math{
	class Vector3;
};
namespace Scene{
using namespace Math;

///衝突判定用三角形集合クラス
/*
Container内部のVertexBufferとIndexBufferを全部使って一つの三角形集合を形成する。
BatchやAnimation等々は単純に見ない。
*/
class CollisionMesh{
public:
	///ニセXMLファイルからコンストラクト
	static CollisionMesh create( const char* filename );
	///ニセXMLのエレメント(Container)からコンストラクト
	static CollisionMesh create( PseudoXml::ConstElement );
	///ロード終了検知。ファイル名から作った場合はtrueが返ることを確認してから使うこと。
	bool isReady();
	///線分と交差判定。
	bool isIntersect( const Vector3& lineOrigin, const Vector3& lineVector ) const;
	///線分と交差判定。最初に当たった時刻をtimeOutに入れる。当たればtrueを返す。
	bool getIntersection( 
		float* timeOut, //0ポインタなら返さない
		Vector3* normalOut, //0ポインタなら返さない
		const Vector3& lineOrigin, 
		const Vector3& lineVector ) const;

	CollisionMesh();
	~CollisionMesh();
	CollisionMesh( const CollisionMesh& );
	operator void*() const;
	CollisionMesh& operator=( const CollisionMesh& );
	void release();
	bool operator==( const CollisionMesh& ) const;
	bool operator!=( const CollisionMesh& ) const;
private:
	class Impl;
	explicit CollisionMesh( Impl* );
	Impl* mImpl;
};

} //namespace Scene
} //namespace GameLib

#endif
