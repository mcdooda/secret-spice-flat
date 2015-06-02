#include "game.h"
#include "loadingstate.h"
#include "gamestate.h"

namespace game
{

void LoadingState::task()
{
	setPriority(HIGH);
	
	m_audioAnalyzer->loadAlgorithms();
	m_audioAnalyzer->analyze();
	std::copy(m_audioAnalyzer->getTicks().begin(), m_audioAnalyzer->getTicks().end(), std::back_inserter(m_game->ticks));
	m_audioAnalyzer->freeAlgorithms();
	m_audioAnalyzer->computeAverageLoudness();
	loadLevel();
	
	m_loaded = true;
}

void LoadingState::enter(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	m_game = game;
	m_audioAnalyzer = &game->audioAnalyzer;
	
	m_audioAnalyzer->setInputFileName(game->argGetString(1));
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
		m_game->getStateMachine()->setState(new GameState());
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
	essentia::Real averageLoudness = m_audioAnalyzer->getAverageLoudness();
	float angleY = 0;
	int i = 0;
	int length = m_game->ticks.size();
	for (std::list<essentia::Real>::iterator it = m_game->ticks.begin(); it != m_game->ticks.end(); it++)
	{
		essentia::Real tick = *it;
		
		essentia::Real prevPrevTick;
		essentia::Real prevTick;
		if (i > 0)
		{
			it--;
			prevTick = *it;
			if (i > 1)
			{
				it--;
				prevPrevTick = *it;
				it++;
			}
			else
				prevPrevTick = prevTick;
			it++;
		}
		else
		{
			prevTick = tick;
			prevPrevTick = prevTick;
		}
			
		essentia::Real nextTick;
		if (i < length - 1)
		{
			it++;
			nextTick = *it;
			it--;
		}
		else
			nextTick = tick;

		flat::geometry::Vector2 center = levelView.getViewMatrix() * zero;
		
		Spectrum* prevPrevSpectrum;
		Spectrum* prevSpectrum;
		Spectrum* currentSpectrum;
		Spectrum* nextSpectrum;
		
		m_audioAnalyzer->getSpectrum(prevPrevTick, &prevPrevSpectrum);
		m_audioAnalyzer->getSpectrum(prevTick, &prevSpectrum);
		m_audioAnalyzer->getSpectrum(tick, &currentSpectrum);
		m_audioAnalyzer->getSpectrum(nextTick, &nextSpectrum);
		
		essentia::Real prevPrevLoudness = prevPrevSpectrum->getLoudness();
		essentia::Real prevLoudness = prevSpectrum->getLoudness();
		essentia::Real loudness = currentSpectrum->getLoudness();
		essentia::Real nextLoudness = nextSpectrum->getLoudness();
		
		float angle = -(loudness - averageLoudness / 2.0f) / (averageLoudness * 8.0f);
		bool strongPeak = false;
		if (prevPrevLoudness < loudness && prevLoudness < loudness && nextLoudness < loudness)
		{
			strongPeak = true;
			angleY += M_PI;
		}

		flat::geometry::Matrix4 matrix4;
		matrix4.scale(20.0f);
		matrix4.rotateZ(angle);
		matrix4.translate(center);

		flat::geometry::Rectangle r = rectangle;
		r.transform(matrix4);
		
		bool addPlatform = loudness > averageLoudness * 0.2f;
		
		if (addPlatform)
			m_game->level.addPlatform(Platform(r, center, angle, angleY, tick, strongPeak ? purple : red, strongPeak));
		
		else
		{
			it = m_game->ticks.erase(it);
			it--;
		}
		
		std::list<essentia::Real>::iterator nextIt = it;
		nextIt++;
		if (nextIt != m_game->ticks.end())
		{
			essentia::Real nextTick = *nextIt;
			flat::geometry::Vector2 move(200.0f * (nextTick - tick), 0);
			levelView.move(move);
		}
		
		i++;
	}
}

void LoadingState::exit(flat::state::Agent* agent)
{
	
}

} // game


