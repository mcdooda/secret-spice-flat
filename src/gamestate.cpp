#include <algorithm>
#include "gamestate.h"

#define CAN_REWIND 1

namespace game
{

void GameState::enter(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	m_music = game->audio->loadMusic(game->argGetString(1));
	m_music->play(1);
	
	m_lastTick = -2.0f;

	m_drawSpectrum = false;
}

void GameState::execute(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	update(game);
	draw(game);
}

void GameState::update(game::Game* game)
{
	if (game->input->keyboard->isJustPressed(K(P)))
	{
		m_music->togglePause();
	}

	if (game->input->keyboard->isJustPressed(K(S)))
	{
		m_drawSpectrum = !m_drawSpectrum;
	}

#if CAN_REWIND
	if (game->input->keyboard->isJustPressed(K(LEFT)))
	{
		m_music->move(-5.0);
	}
	else if (game->input->keyboard->isJustPressed(K(RIGHT)))
	{
		m_music->move(5.0);
	}

	if (game->input->keyboard->isJustPressed(K(R)))
	{
		m_music->rewind();
		m_music->play(1);
	}
#endif

	float currentTime = m_music->getPosition();

	std::cout << "currentTime = " << currentTime << std::endl;
	
	if (!ticks.empty() && currentTime > *ticks.begin())
	{
#if CAN_REWIND
		m_lastTick = *ticks.begin();
		for (essentia::Real tick : ticks)
		{
			if (tick < currentTime)
				m_lastTick = tick;

			else
				break;
		}
#else
		m_lastTick = *ticks.begin();
		ticks.pop_front();
#endif
	}
	
	level.fadeOldPlatforms(currentTime);
#if !CAN_REWIND
	level.removeOldPlatforms(currentTime - 7.0f);
#endif
	
	Platform* previousPlatform;
	Platform* nextPlatform;
	level.getCurrentPlatforms(currentTime, &previousPlatform, &nextPlatform);
	
	if (previousPlatform != NULL && nextPlatform != NULL)
	{
		float viewAngle;
		float viewAngleY;
		flat::geometry::Vector2 viewPosition;
		
		float alpha;
		if (previousPlatform != nextPlatform)
		{
			alpha = (currentTime - previousPlatform->getTime()) / (nextPlatform->getTime() - previousPlatform->getTime());
			alpha = powf(alpha, 5.0f);
		}
		else
			alpha = 0;
		
		viewPosition = (previousPlatform->getCenter() + (nextPlatform->getCenter() - previousPlatform->getCenter()) * alpha) * -1;
		viewAngle = -(previousPlatform->getAngle() + (nextPlatform->getAngle() - previousPlatform->getAngle()) * alpha);
		viewAngleY = -(previousPlatform->getAngleY() + (nextPlatform->getAngleY() - previousPlatform->getAngleY()) * alpha);
		
		game->view.reset();
		game->view.move(viewPosition);
		game->view.rotateZ(viewAngle);
		game->view.rotateY(viewAngleY);
		
		audioAnalyzer.getSpectrum(currentTime, m_currentSpectrum);
	}
}

void GameState::draw(game::Game* game)
{
	float currentTime = m_music->getPosition();
	float flashDuration = 0.1f;

	float flashValue = 0.0f;
	game->levelPass.use();
	if (currentTime - m_lastTick < flashDuration)
	{
		flashValue = ((currentTime - m_lastTick) / flashDuration) * 0.3f + 0.7f;
		//game->video->setClearColor(flat::video::Color(flashValue, 0.f, 0.f, 1.0f));
		game->video->setClearColor(flat::video::Color::WHITE);
	}
	else if (m_currentSpectrum != NULL)
	{
		float gray = 1.0f - m_currentSpectrum->getMax().y;
		game->video->setClearColor(flat::video::Color(gray));
	}
	else
		game->video->setClearColor(flat::video::Color::WHITE);

	game->video->clear();

	game->levelVpMatrixUniform.setMatrix4(game->view.getViewProjectionMatrix());
	level.draw(currentTime + 7.0f, game->levelPositionAttribute, game->levelUvAttribute, game->levelColorUniform);

	game->renderProgram.use(game->video->window);
	game->renderCurrentTimeUniform.setFloat(currentTime);
	game->renderFlashValueUniform.setFloat(flashValue);
	game->renderProgram.draw();

	game->interfaceProgram.use(game->video->window);
	game->levelColorUniform.setColor(flat::video::Color(1.0f, 0.0f, 0.0f, 1.0f));
	const flat::geometry::Vector2 windowSize = game->video->window->getSize();
	float cursorPosition = currentTime / audioAnalyzer.getDuration();
	cursorPosition = cursorPosition > 1.0f ? 1.0f : cursorPosition;
	flat::geometry::Rectangle r(flat::geometry::Vector2(2.0f, 2.0f), flat::geometry::Vector2((windowSize.x - 4.0f) * cursorPosition, 2.0f));
	game->levelVpMatrixUniform.setMatrix4(game->interfaceView.getViewProjectionMatrix());
	r.draw(game->levelPositionAttribute, game->levelUvAttribute);

	if (m_drawSpectrum)
		drawCurrentSpectrum(game);
}

void GameState::exit(flat::state::Agent* agent)
{
	//game::Game* game = (game::Game*) agent;
	
	FLAT_DELETE(m_music);
}

void GameState::drawCurrentSpectrum(game::Game* game)
{
	game->interfaceProgram.use(game->video->window);

	const flat::geometry::Vector2 windowSize = game->video->window->getSize();

	const float spectrumX = 10.0f;
	const float spectrumY = 10.0f;
	const float spectrumWidth = windowSize.x - 20.0f;
	const float spectrumHeight = 200.0f;
	const float padding = 2.0f;

	{
		game->levelColorUniform.setColor(flat::video::Color(0.0f, 0.0f, 0.0f, 1.0f));
		flat::geometry::Rectangle r(flat::geometry::Vector2(spectrumX - padding, spectrumY - padding), flat::geometry::Vector2(spectrumWidth + padding * 2.0f, spectrumHeight + padding * 2.0f));
		r.draw(game->levelPositionAttribute, game->levelUvAttribute);
	}

	if (m_currentSpectrum)
	{
		game->levelColorUniform.setColor(flat::video::Color(0.0f, 1.0f, 0.0f, 1.0f));
		game->levelVpMatrixUniform.setMatrix4(game->interfaceView.getViewProjectionMatrix());
		const std::vector<essentia::Real>& spectrumData = m_currentSpectrum->getSpectrum();
		essentia::Real max = 1.f / log10(audioAnalyzer.getMax());
		float x = 0;
		for (essentia::Real y : spectrumData)
		{
			flat::geometry::Rectangle r(flat::geometry::Vector2(spectrumX + x / spectrumData.size() * spectrumWidth, spectrumY), flat::geometry::Vector2(1.0f, 1.f / log10(y) / max * spectrumHeight));
			r.draw(game->levelPositionAttribute, game->levelUvAttribute);
			++x;
		}
	}
}

} // game


