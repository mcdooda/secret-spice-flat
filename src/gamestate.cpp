#include "gamestate.h"

namespace game
{

void GameState::enter(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	m_music.reset(game->audio->loadMusic(game->argGetString(1)));
	m_music->play(1);
	
	m_beginTime = game->time->getTime();
	m_lastTick = -2.0f;
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
		game->time->togglePause();

		if (game->time->isPaused())
			flat::audio::Music::pause();

		else
			flat::audio::Music::resume();
	}

	float currentTime = game->time->getTime() - m_beginTime;
	
	if (!ticks.empty() && currentTime > *ticks.begin())
	{
		m_lastTick = *ticks.begin();
		ticks.pop_front();
	}
	
	level.fadeOldPlatforms(currentTime);
	level.removeOldPlatforms(currentTime - 7.0f);
	
	Platform* previousPlatform;
	Platform* nextPlatform;
	level.getCurrentPlatforms(currentTime, &previousPlatform, &nextPlatform);
	
	if (previousPlatform && nextPlatform)
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
		
		audioAnalyzer.getSpectrum(currentTime, &m_currentSpectrum);
	}
}

void GameState::draw(game::Game* game)
{
	float currentTime = game->time->getTime() - m_beginTime;
	float flashDuration = 0.1f;

	float flashValue = 0.0f;
	game->levelPass.use();
	if (currentTime - m_lastTick < flashDuration)
	{
		flashValue = ((currentTime - m_lastTick) / flashDuration) * 0.3f + 0.7f;
		game->video->setClearColor(flat::video::Color(flashValue, 0.f, 0.f, 1.0f));
		//game->video->setClearColor(flat::video::Color::WHITE);
	}
	else if (m_currentSpectrum)
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
}

void GameState::exit(flat::state::Agent* agent)
{
	//game::Game* game = (game::Game*) agent;
	
	
}

} // game


