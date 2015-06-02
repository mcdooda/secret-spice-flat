#include "game.h"
#include "globalstate.h"

namespace game
{

void GlobalState::enter(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	game->video->window->setTitle("BEATSHAPT");
	
	game->frameBuffer.setSize(game->video->window->getSize() * 2.0f);
	
	game->levelPass.load(&game->frameBuffer, "rsrc/shader/levelpass.frag", "rsrc/shader/levelpass.vert");
	const flat::video::Texture& screenTexture = game->frameBuffer.addTexture("screen");
	
	game->renderProgram.load("rsrc/shader/renderprogram.frag", "rsrc/shader/renderprogram.vert");
	game->renderProgram.addInputTexture(screenTexture);
	
	game->levelPositionAttribute = game->levelPass.getAttribute("position");
	game->levelUvAttribute = game->levelPass.getAttribute("uv");
	game->levelVpMatrixUniform = game->levelPass.getUniform("vpMatrix");
	game->levelColorUniform = game->levelPass.getUniform("color");
	
	game->renderPositionAttribute = game->renderProgram.getAttribute("position");
	game->renderUvAttribute = game->renderProgram.getAttribute("uv");
	game->renderCurrentTimeUniform = game->renderProgram.getUniform("currentTime");
	game->renderFlashValueUniform = game->renderProgram.getUniform("flashValue");
	
	resetViews(game);
	
	AudioAnalyzer::open();
}

void GlobalState::execute(flat::state::Agent* agent)
{
	game::Game* game = (game::Game*) agent;
	
	if (game->input->window->isResized())
		resetViews(game);
		
	if (game->input->keyboard->isJustPressed(K(ESCAPE)))
		game->stop();
}

void GlobalState::exit(flat::state::Agent* agent)
{
	//game::Game* game = (game::Game*) agent;
	
	AudioAnalyzer::close();
}

void GlobalState::resetViews(game::Game* game)
{
	const flat::geometry::Vector2 windowSize = game->video->window->getSize();
	
	game->view.reset();
	game->view.updateProjection(windowSize);
	
	game->interfaceView.reset();
	game->interfaceView.move(windowSize / -2.0f);
	game->interfaceView.updateProjection(windowSize);
}

} // game


