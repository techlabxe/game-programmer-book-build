#include "GameLib/Framework.h"
#include "GameLib/Input/Manager.h"
#include "GameLib/Input/Keyboard.h"
#include "GameLib/Math/Vector3.h"
#include "GameLib/Math/Matrix34.h"
#include "GameLib/Math/Matrix44.h"
#include "GameLib/Math/Functions.h"
#include "GameLib/Scene/StringRenderer.h"
#include "GameLib/Scene/Font.h"
#include <sstream>
using namespace std;
using namespace GameLib::Math;
using namespace GameLib::Graphics;
using namespace GameLib::Scene;

int gCount;
Font gFont0;
Font gFont1;
StringRenderer gStringRenderer;

namespace GameLib{
	void Framework::configure( Configuration* c ){
		c->enableVSync( true );
	}
	void Framework::update(){
		if ( gCount == 0 ){
			//Windowsフォント生成(アンチエイリアス付き)
			gFont0 = Font::create( "ＭＳ ゴシック", 32, false, false, false, true ); //MSは全角、次のスペースは半角。気をつけよう。
			//Windowsフォント生成(アンチエイリアス付き)
			gFont1 = Font::create( "ＭＳ 明朝", 32, false, false, false, true ); //MSは全角、次のスペースは半角。気をつけよう。
			//文字描画クラス(文字数、addの呼び出し回数)
			gStringRenderer = StringRenderer::create( 2048, 128 );
		}
		//描画してみる。
		ostringstream oss;
		oss << "フレームレート: " << frameRate();
		//trueは自動折り返し機能。falseにすれば折り返さない
		gStringRenderer.setFont( gFont0 );
		gStringRenderer.add( gCount % 640, 0, oss.str().c_str(), 0xffffffff, true );
		oss.str( "" );
		oss << "　木理（もくめ）美（うるは）しき槻胴（けやきどう）、縁にはわざと赤樫を用ひたる岩畳（がんでふ）作りの長火鉢に対ひて話し敵（がたき）もなく唯一人、少しは淋しさうに坐り居る三十前後の女、男のやうに立派な眉を何日（いつ）掃ひしか剃つたる痕の青と、見る眼も覚むべき雨後の山の色をとゞめて翠（みどり）のひ一トしほ床しく、鼻筋つんと通り眼尻キリヽと上り、洗ひ髪をぐる／＼と酷（むご）く丸（まろ）めて引裂紙をあしらひに一本簪（いつぽんざし）でぐいと留めを刺した色気無の様はつくれど、";
		//trueは自動折り返し機能。falseにすれば折り返さない
		gStringRenderer.setFont( gFont1 );
		gStringRenderer.add( gCount % 640, 100, oss.str().c_str(), 0xffffffff, true );

		gStringRenderer.draw();
		++gCount;

		if ( isEndRequested() ){
			gStringRenderer.release(); //グローバルなので開放処理が必要
			gFont0.release();
			gFont1.release();
		}
	}
}
