#ifndef GAME_SPECTRUM_H
#define GAME_SPECTRUM_H

#include <essentia/essentia.h>

#include <flat.h>

namespace game
{

class Spectrum
{
	public:
		Spectrum(const std::vector<essentia::Real>& spectrum, essentia::Real strongPeak, essentia::Real loudness);
		~Spectrum();
		
		inline const flat::geometry::Vector2& getMax() const { return m_max; }
		inline const std::vector<essentia::Real>& getSpectrum() const { return m_spectrum; }
		inline essentia::Real getStrongPeak() const { return m_strongPeak; }
		inline essentia::Real getLoudness() const { return m_loudness; }
		
	private:
		void computeMax();
		
	private:
		std::vector<essentia::Real> m_spectrum;
		essentia::Real m_strongPeak;
		essentia::Real m_loudness;
		flat::geometry::Vector2 m_max;
};

} // game

#endif // GAME_SPECTRUM_H


