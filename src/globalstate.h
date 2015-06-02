#ifndef GAME_GLOBALSTATE_H
#define GAME_GLOBALSTATE_H

#include <flat.h>

#include "game.h"

namespace game
{

class GlobalState : public flat::state::State
{
	public:
		virtual void enter(flat::state::Agent* agent);
		virtual void execute(flat::state::Agent* agent);
		virtual void exit(flat::state::Agent* agent);
		
		void resetViews(game::Game* game);
};

}

#endif // GAME_GLOBALSTATE_H


