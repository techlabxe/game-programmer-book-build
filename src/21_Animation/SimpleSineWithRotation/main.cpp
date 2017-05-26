#include "GameLib/Framework.h"

#include "Library/GraphicsDatabase.h"
#include "Library/Model.h"
#include "Library/Vector3.h"
#include "Library/Matrix44.h"
#include "Library/Matrix34.h"
#include "GameLib/Math.h"
using namespace GameLib;
using namespace std;

GraphicsDatabase* gDatabase;
Model* gUpperModel;
Model* gLowerModel;
int gCount;
Vector3 gLightVector;
Vector3 gLightColor( 0.8, 0.7, 0.6 );
Vector3 gAmbient( 0.2, 0.3, 0.4 );
Vector3 gEyePosition;

namespace GameLib{
	void Framework::update(){
		if ( !gDatabase ){
			setFrameRate( 60 );
			gDatabase = new GraphicsDatabase( "cube.txt" );
			gUpperModel = gDatabase->createModel( "cube" );
			gLowerModel = gDatabase->createModel( "cube" );
		}
		gUpperModel->setPosition( Vector3( 0.0, 1.0, 0.0 ) );
		double y = 4.0 + sin( gCount * 5.0 );
		gLowerModel->setPosition( Vector3( 0.0, y, 0.0 ) );
		//‰ñ“]‚µ‚Ä‚Ý‚é
		gUpperModel->setAngle( Vector3( 0.0, 0.0, 45.0 ) );
		gLowerModel->setAngle( Vector3( 0.0, 0.0, 45.0 ) );

		//ƒJƒƒ‰‚Æƒ‰ƒCƒg‚ð“®‚©‚µ‚Ä‚Ý‚é
		gLightVector.x = sin( gCount ) * cos( gCount );
		gLightVector.y = sin( gCount ) * sin( gCount );
		gLightVector.z = cos( gCount );
		gEyePosition.x = 20.0 * cos( gCount * 0.5 );
		gEyePosition.y = 10.0;
		gEyePosition.z = 20.0 * sin( gCount * 0.5 );

		Matrix44 pm;
		pm.setPerspectiveTransform( 45.0, width(), height(), 1.0, 10000.0 );
		Matrix34 vm;
		vm.setViewTransform( gEyePosition, Vector3( 0.0, 0.0, 0.0 ) );
		pm *= vm;
		gUpperModel->draw( pm, gLightVector, gLightColor, gAmbient );
		gLowerModel->draw( pm, gLightVector, gLightColor, gAmbient );

		++gCount;
		if ( isEndRequested() ){
			SAFE_DELETE( gUpperModel );
			SAFE_DELETE( gLowerModel );
			SAFE_DELETE( gDatabase );
		}
	}
}
