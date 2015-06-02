#include <iostream>
#include <flat/game.h>
#include "game.h"
#include "globalstate.h"
#include "loadingstate.h"
#include "gamestate.h"

namespace game
{

Game::Game(const std::vector<std::string>& args) : flat::Game(args)
{
	getStateMachine()->setGlobalState(new GlobalState());
    getStateMachine()->setState(new LoadingState());
}

Game::~Game()
{
	
}

void Game::checkArgs()
{
	argCheckString(1);
}

} // game


