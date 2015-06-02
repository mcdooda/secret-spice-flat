#ifndef GAME_GAMESTATE_H
#define GAME_GAMESTATE_H

#include <flat.h>
#include "game.h"

namespace game
{

class GameState : public flat::state::State
{
	public:
		virtual void enter(flat::state::Agent* agent);
		virtual void execute(flat::state::Agent* agent);
		void update(game::Game* game);
		void draw(game::Game* game);
		virtual void exit(flat::state::Agent* agent);
};

}

#endif // GAME_GAMESTATE_H


