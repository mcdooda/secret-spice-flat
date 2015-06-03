#ifndef GAME_GAME_H
#define GAME_GAME_H

#include <flat/game.h>

#include "level.h"
#include "audioanalyzer.h"

namespace game
{

class Game : public flat::Game
{
	public:
		Game(const std::vector<std::string>& args);
		virtual ~Game();
		
		virtual void checkArgs();
		
	public:
		// music
		flat::audio::Music* music;
		AudioAnalyzer audioAnalyzer;
		std::list<essentia::Real> ticks;
		Spectrum* currentSpectrum;
		
		// views
		flat::video::View view;
		flat::video::View interfaceView;
		
		// passes
		flat::video::FrameBuffer frameBuffer;
		flat::video::Pass levelPass;
		flat::video::Render renderProgram;
		flat::video::Program interfaceProgram;
		
		// attributes & uniforms
		flat::video::Attribute levelPositionAttribute;
		flat::video::Attribute levelUvAttribute;
		flat::video::Uniform levelVpMatrixUniform;
		flat::video::Uniform levelColorUniform;
		
		flat::video::Attribute renderPositionAttribute;
		flat::video::Attribute renderUvAttribute;
		flat::video::Uniform renderCurrentTimeUniform;
		flat::video::Uniform renderFlashValueUniform;
		
		// game objects
		Level level;
		
		float beginTime;
		float lastTick;
		float rotateDirection;
};

} // game

#endif // GAME_GAME_H


