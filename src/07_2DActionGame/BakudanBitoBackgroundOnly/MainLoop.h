#ifndef INCLUDED_MAINLOOP_H
#define INCLUDED_MAINLOOP_H

namespace Sequence{
	class TitleSequence;
	class GameSequence;
	class GameOverSequence;
	class EndingSequence;
}

class MainLoop{
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
	Mode getMode() const;
	void setMode( Mode );

	static void create();
	static void destroy();
	static MainLoop* getInstance();
private:
	MainLoop();
	~MainLoop();

	NextSequence mNextSequence;
	Mode mMode;

	Sequence::TitleSequence* mTitle;
	Sequence::GameSequence* mGame;
	Sequence::GameOverSequence* mGameOver;
	Sequence::EndingSequence* mEnding;

	static MainLoop* mInstance;
};

#endif