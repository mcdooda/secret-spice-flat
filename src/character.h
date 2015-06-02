#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include <iostream>

#include <flat.h>

#include "spectrum.h"

namespace game
{

class Character
{
	public:
		Character();
		~Character();
		
		void setSpectrum(Spectrum* spectrum);
		
		void draw(flat::video::Attribute vertexAttribute, const flat::video::Uniform& colorUniform, const flat::video::Uniform& centerUniform);
		
		inline void setPosition(const flat::geometry::Vector2& position) { m_position = position; }
		inline const flat::geometry::Vector2& getPosition() const { return m_position; }
		
	private:
		flat::geometry::Polygon m_shape;
		flat::geometry::Vector2 m_position;
};

} // game

#endif // GAME_CHARACTER_H


