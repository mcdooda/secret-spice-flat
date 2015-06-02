#include <iostream>
#include "platform.h"

namespace game
{

Platform::Platform(const flat::geometry::Rectangle& rectangle, const flat::geometry::Vector2& center, float angle, float angleY, float time, const flat::video::Color& color, bool strongPeak) :
	m_rectangle(rectangle),
	m_center(center),
	m_angle(angle),
	m_angleY(angleY),
	m_time(time),
	m_color(color),
	m_strongPeak(strongPeak)
{
	
}

Platform::~Platform()
{
	
}

void Platform::draw(flat::video::Attribute positionAttribute, flat::video::Attribute uvAttribute, const flat::video::Uniform& colorUniform)
{
	colorUniform.setColor(m_color);
	m_rectangle.draw(positionAttribute, uvAttribute);
}

} // game


