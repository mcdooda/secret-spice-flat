#include "spectrum.h"

namespace game
{

Spectrum::Spectrum(const std::vector<essentia::Real>& spectrum, essentia::Real strongPeak, essentia::Real loudness) :
	m_spectrum(spectrum),
	m_strongPeak(strongPeak),
	m_loudness(loudness)
{
	computeMax();
}

Spectrum::~Spectrum()
{
	
}

void Spectrum::computeMax()
{
	float max = 0;
	int i = 0;
	int imax = 0;
	for (essentia::Real y : m_spectrum)
	{
		if (y > max)
		{
			max = y;
			imax = i;
		}
		i++;
	}
	m_max.x = static_cast<float>(imax / m_spectrum.size());
	m_max.y = max;
}

} // game


