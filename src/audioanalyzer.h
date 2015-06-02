#ifndef GAME_AUDIO_ANALYZER_H
#define GAME_AUDIO_ANALYZER_H

#include <string>
#include <essentia/essentia.h>
#include <essentia/algorithmfactory.h>

#include "spectrum.h"

namespace game
{

class AudioAnalyzer
{
	public:
		AudioAnalyzer();
		~AudioAnalyzer();
		
		static void open();
		static void close();
		
		void setInputFileName(std::string inputFileName) { m_inputFileName = inputFileName; }
		void loadAlgorithms();
		void analyze();
		void computeAverageLoudness();
		void freeAlgorithms();
		
		essentia::Real getDuration() const { return m_duration; }
		const std::vector<essentia::Real>& getTicks() const { return m_ticks; }
		
		void getSpectrum(float time, Spectrum** spectrum) const;
		
		inline essentia::Real getAverageLoudness() const { return m_averageLoudness; }
		
	private:
		std::string m_inputFileName;
		
		// algorithms
		essentia::standard::Algorithm* m_frameCutterAlgorithm;
		essentia::standard::Algorithm* m_windowingAlgorithm;
		essentia::standard::Algorithm* m_spectrumAlgorithm;
		essentia::standard::Algorithm* m_loudnessAlgorithm;
		essentia::standard::Algorithm* m_strongPeakAlgorithm;
		
		// algorithms' data
		std::vector<essentia::Real>* m_audioBuffer;
		std::vector<essentia::Real>* m_frame;
		std::vector<essentia::Real>* m_windowedFrame;
		std::vector<essentia::Real>* m_spectrumBuffer;
		essentia::Real m_loudness;
		essentia::Real m_strongPeakValue;
		
		// generated data
		essentia::Real m_duration;
		std::vector<essentia::Real> m_ticks;
		std::vector<Spectrum> m_spectrums;
		essentia::Real m_averageLoudness;
};

} // game

#endif // GAME_AUDIO_ANALYZER_H


