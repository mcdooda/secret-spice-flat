#include <iostream>
#include "level.h"

namespace game
{

Level::Level()
{
	
}

Level::~Level()
{
	
}

void Level::draw(float maxTime, flat::video::Attribute positionAttribute, flat::video::Attribute uvAttribute, const flat::video::Uniform& colorUniform)
{
	for (std::list<Platform>::iterator it = m_platforms.begin(); it != m_platforms.end() && it->getTime() < maxTime; it++)
		it->draw(positionAttribute, uvAttribute, colorUniform);
}

void Level::getCurrentPlatforms(float time, Platform** previousPlatform, Platform** nextPlatform)
{
	if (m_platforms.empty())
	{
		*previousPlatform = NULL;
		*nextPlatform = NULL;
	}
	else if (time < m_platforms.front().getTime())
	{
		*previousPlatform = &m_platforms.front();
		*nextPlatform = *previousPlatform;
	}
	else
	{
		*previousPlatform = NULL;
		*nextPlatform = NULL;
		for (std::list<Platform>::iterator it = m_platforms.begin(); it != m_platforms.end(); it++)
		{
			if (it->getTime() > time)
			{
				*nextPlatform = &(*it);
				it--;
				*previousPlatform = &(*it);
				break;
			}
		}
		if (*previousPlatform == NULL)
		{
			*previousPlatform = &m_platforms.back();
			*nextPlatform = *previousPlatform;
		}
	}
}

void Level::addPlatform(Platform platform)
{
	m_platforms.push_back(platform);
}

void Level::removeOldPlatforms(float maxTime)
{
	while (m_platforms.begin() != m_platforms.end() && m_platforms.begin()->getTime() < maxTime)
		m_platforms.pop_front();
}

void Level::fadeOldPlatforms(float maxTime)
{
	for (std::list<Platform>::iterator it = m_platforms.begin(); it != m_platforms.end() && it->getTime() < maxTime; it++)
		it->setColor(flat::video::Color(0.0f, 0.0f, 1.0f, 1.0f));
}

} // game


