#ifndef INCLUDED_SEQUENCE_PARENT_H
#define INCLUDED_SEQUENCE_PARENT_H

namespace Sequence{

namespace Game{
	class Parent;
}
class Title;
class GameOver;
class Ending;

class Parent{
public:
	enum NextSequence{
		NEXT_TITLE,
		NEXT_GAME,
		NEXT_GAME_OVER,
		NEXT_ENDING,

		NEXT_NONE,
	};
	enum Mode{
		MODE_1P,
		MODE_2P,
		MODE_NONE,
	};
	void update();
	void moveTo( NextSequence );
	Mode mode() const;
	void setMode( Mode );

	static void create();
	static void destroy();
	static Parent* instance();
private:
	Parent();
	~Parent();

	NextSequence mNextSequence;
	Mode mMode;

	Title* mTitle;
	Game::Parent* mGame;
	GameOver* mGameOver;
	Ending* mEnding;

	static Parent* mInstance;
};

} //namespace Sequence

#endif
