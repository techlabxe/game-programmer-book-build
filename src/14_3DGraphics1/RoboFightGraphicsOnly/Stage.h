#ifndef INCLUDED_STAGE_H
#define INCLUDED_STAGE_H


class Matrix44;
namespace GameLib{
	class Texture;
}

class Stage{
public:
	Stage();
	~Stage();
	void draw( const Matrix44& perspectiveViewMatrix ) const;
private:
	GameLib::Texture* mTexture;
};

#endif
