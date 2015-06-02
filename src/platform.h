#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

#include <flat.h>

namespace game
{

class Platform
{
	public:
		Platform(const flat::geometry::Rectangle& rectangle, const flat::geometry::Vector2& center, float angle, float angleY, float time, const flat::video::Color& color, bool strongPeak);
		~Platform();
		
		void draw(flat::video::Attribute positionAttribute, flat::video::Attribute uvAttribute, const flat::video::Uniform& colorUniform);
		
		inline float getTime() const { return m_time; }
		
		inline const flat::video::Color& getColor() const { return m_color; }
		inline void setColor(const flat::video::Color& color) { m_color = color; }
		
		inline bool getStrongPeak() const { return m_strongPeak; }
		
		inline const flat::geometry::Vector2 getSize() const { return m_rectangle.getSize(); }
		inline void setSize(const flat::geometry::Vector2& size) { m_rectangle.setSize(size); }
		
		inline const flat::geometry::Vector2& getCenter() const { return m_center; }
		
		inline float getAngle() const { return m_angle; }
		inline float getAngleY() const { return m_angleY; }
		
	private:
		flat::geometry::Rectangle m_rectangle;
		flat::geometry::Vector2 m_center;
		float m_angle;
		float m_angleY;
		float m_time;
		flat::video::Color m_color;
		bool m_strongPeak;
};

} // game

#endif // GAME_LEVEL_G


