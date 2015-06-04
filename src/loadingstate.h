#ifndef GAME_LOADINGSTATE_H
#define GAME_LOADINGSTATE_H

#include <flat.h>

namespace game
{
class GameState;

class LoadingState : public flat::thread::Thread, public flat::state::State
{
	public:
		virtual void task();
		
		virtual void enter(flat::state::Agent* agent);
		virtual void execute(flat::state::Agent* agent);
		void update();
		void draw();
		void loadLevel();
		virtual void exit(flat::state::Agent* agent);
		
	private:
		volatile bool m_loaded;
		Game* m_game;
		GameState* m_gameState;
};

}

#endif // GAME_LOADINGSTATE_H


