#include "game.h"
#include "loadingstate.h"
#include "gamestate.h"

namespace game
{

void LoadingState::task()
{
	setPriority(HIGH);

	AudioAnalyzer& audioAnalyzer = m_gameState->audioAnalyzer;
	
	audioAnalyzer.loadAlgorithms();
	audioAnalyzer.analyze();
	std::copy(audioAnalyzer.getTicks().begin(), audioAnalyzer.getTicks().end(), std::back_inserter(m_gameState->ticks));
	audioAnalyzer.freeAlgorithms();
	audioAnalyzer.computeAverageLoudness();
	loadLevel();
	
	m_loaded = true;
}

void LoadingState::enter(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	m_game = game;
	
	m_gameState = new GameState();
	m_gameState->audioAnalyzer.setInputFileName(game->argGetString(1));

	m_loaded = false;
	run();
}

void LoadingState::execute(flat::state::Agent* agent)
{
	draw();
	update();
}

void LoadingState::update()
{
	if (m_loaded)
	{
		wait();
		m_game->getStateMachine()->setState(m_gameState);
	}
}

void LoadingState::draw()
{
	m_game->levelPass.use();
	m_game->video->setClearColor(flat::video::Color::BLACK);
	m_game->video->clear();
	m_game->levelVpMatrixUniform.setMatrix4(m_game->view.getViewProjectionMatrix());
	flat::geometry::Rectangle r(flat::geometry::Vector2(-1.0f, -1.0f), flat::geometry::Vector2(2.0f, 2.0f));
	flat::geometry::Matrix4 matrix4;
	matrix4.scale(20.0f + fmodf(m_game->time->getTime() * 4.0f, 10.0f));
	matrix4.rotateZ(m_game->time->getTime());
	r.transform(matrix4);
	m_game->levelColorUniform.setColor(flat::video::Color(0.5f - fmodf(m_game->time->getTime() * 0.2f, 0.5f)));
	r.draw(m_game->levelPositionAttribute);
	
	m_game->renderProgram.use(m_game->video->window);
	m_game->renderCurrentTimeUniform.setFloat(0.0f);
	m_game->renderFlashValueUniform.setFloat(0.0f);
	m_game->renderProgram.draw();
}

void LoadingState::loadLevel()
{
	flat::video::View levelView;
	flat::geometry::Vector2 zero;
	flat::video::Color red(1.0f, 0.0f, 0.0f, 1.0f);
	flat::video::Color purple(1.0f, 0.0f, 1.0f, 1.0f);
	flat::geometry::Rectangle rectangle(flat::geometry::Vector2(-1.0f, -1.0f), flat::geometry::Vector2(2.0f, 2.0f));
	AudioAnalyzer& audioAnalyzer = m_gameState->audioAnalyzer;
	essentia::Real averageLoudness = audioAnalyzer.getAverageLoudness();
	essentia::Real maxLoudness = audioAnalyzer.getMaxLoudness();
	float angleY = 0;
	int i = 0;
	int length = m_gameState->ticks.size();
	std::list<essentia::Real>::iterator it = m_gameState->ticks.begin();
	essentia::Real prevPrevTick = 0.f;
	essentia::Real prevTick = *it;
	++it;
	essentia::Real tick = *it;
	for (; it != m_gameState->ticks.end(); ++it)
	{
		prevPrevTick = prevTick;
		prevTick = tick;
		tick = *it;

		essentia::Real nextTick;
		if (i < length - 1)
		{
			++it;
			nextTick = *it;
			--it;
		}
		else
			nextTick = tick;

		flat::geometry::Vector2 center = levelView.getViewMatrix() * zero;
		
		const Spectrum* prevPrevSpectrum;
		const Spectrum* prevSpectrum;
		const Spectrum* currentSpectrum;
		const Spectrum* nextSpectrum;
		
		audioAnalyzer.getSpectrum(prevPrevTick, prevPrevSpectrum);
		audioAnalyzer.getSpectrum(prevTick, prevSpectrum);
		audioAnalyzer.getSpectrum(tick, currentSpectrum);
		audioAnalyzer.getSpectrum(nextTick, nextSpectrum);
		
		essentia::Real prevPrevLoudness = prevPrevSpectrum->getLoudness();
		essentia::Real prevLoudness = prevSpectrum->getLoudness();
		essentia::Real loudness = currentSpectrum->getLoudness();
		essentia::Real nextLoudness = nextSpectrum->getLoudness();

		essentia::Real localAverageLoudness = (prevPrevLoudness + prevLoudness + loudness + nextLoudness) / 4.f;
		
		float angle = -(loudness - localAverageLoudness / 2.0f) / (localAverageLoudness * 8.0f);
		bool strongPeak = false;
		if (prevPrevLoudness < loudness && prevLoudness < loudness && nextLoudness < loudness)
		{
			float angle = -(loudness - averageLoudness / 2.0f) / (averageLoudness * 8.0f);
			bool strongPeak = false;
			if (prevPrevLoudness < loudness && prevLoudness < loudness && nextLoudness < loudness)
			{
				strongPeak = true;
				angleY += M_PI;
			}

			flat::geometry::Vector2 max = currentSpectrum->getMax();

			flat::geometry::Matrix4 matrix4;
			matrix4.scale(20.f * (0.5f - max.x) * 2.f, 20.f * (0.5f - max.x) * 2.f);
			matrix4.rotateZ(angle);
			matrix4.translate(center);

			flat::geometry::Rectangle r = rectangle;
			r.transform(matrix4);

			bool addPlatform = loudness > averageLoudness * 0.2f && loudness > maxLoudness * 0.1f;

			if (addPlatform)
				m_gameState->level.addPlatform(Platform(r, center, angle, angleY, tick, strongPeak ? purple : red, strongPeak));
		}
		else
		{
			it = m_gameState->ticks.erase(it);
			--it;
		}
		
		flat::geometry::Vector2 move(200.0f * (nextTick - tick), 0);
		levelView.move(move);
		
		++i;
	}
}

void LoadingState::exit(flat::state::Agent* agent)
{
	
}

} // game


