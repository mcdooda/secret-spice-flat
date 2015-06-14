#ifndef GAME_GAMESTATE_H
#define GAME_GAMESTATE_H

#include <memory>
#include <flat.h>
#include "game.h"

namespace game
{
class Spectrum;

class GameState : public flat::state::State
{
	public:
		virtual void enter(flat::state::Agent* agent);
		virtual void execute(flat::state::Agent* agent);
		void update(game::Game* game);
		void draw(game::Game* game);
		virtual void exit(flat::state::Agent* agent);

	private:
		void drawCurrentSpectrum(game::Game* game);

	public:
		AudioAnalyzer audioAnalyzer;
		std::list<essentia::Real> ticks;
		Level level;

	private:
		flat::audio::Music* m_music;
		const Spectrum* m_currentSpectrum;
		float m_lastTick;
		bool m_drawSpectrum;
};

}

#endif // GAME_GAMESTATE_H


