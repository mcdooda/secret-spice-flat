#include "character.h"

namespace game
{

Character::Character()
{
	
}

Character::~Character()
{
	
}

void Character::setSpectrum(Spectrum* spectrum)
{
	if (spectrum != NULL)
	{
		const std::vector<essentia::Real>& spectrumData = spectrum->getSpectrum();
		float x = 0;
		float dx = 1.0f / spectrumData.size() * M_PI * 2;
		std::vector<flat::geometry::Vector2> shapeVertices(spectrumData.size());
		for (std::vector<essentia::Real>::const_iterator it = spectrumData.begin(); it != spectrumData.end(); it++)
		{
			float y = *it;
			shapeVertices.push_back(flat::geometry::Vector2(x, y));
			x += dx;
		}
		m_shape.setVertices(shapeVertices);
	}
}

void Character::draw(flat::video::Attribute vertexAttribute, const flat::video::Uniform& colorUniform, const flat::video::Uniform& centerUniform) const
{
	colorUniform.setColor(flat::video::Color::WHITE);
	centerUniform.setVector2(flat::geometry::Vector2(0, 0));
	m_shape.draw(vertexAttribute);
}

} // game
