#include <essentia/essentiamath.h>
#include "audioanalyzer.h"

namespace game
{

using namespace essentia;

AudioAnalyzer::AudioAnalyzer()
{
	
}

AudioAnalyzer::~AudioAnalyzer()
{
	
}

void AudioAnalyzer::open()
{
	essentia::init();
}

void AudioAnalyzer::close()
{
	essentia::shutdown();
}

void AudioAnalyzer::loadAlgorithms()
{
	essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
	
	/*
	 * MonoLoader
	 */
	essentia::standard::Algorithm* monoLoaderAlgorithm = factory.create(
		"MonoLoader",
		"filename", m_inputFileName
	);
	m_audioBuffer = new std::vector<essentia::Real>();
	
	// -->
	monoLoaderAlgorithm->output("audio").set(*m_audioBuffer);
	
	monoLoaderAlgorithm->compute();
	delete monoLoaderAlgorithm;
	
	/*
	 * Duration
	 */
	essentia::standard::Algorithm* durationAlgorithm = factory.create("Duration");
	
	// <--
	durationAlgorithm->input("signal").set(*m_audioBuffer);
	
	// -->
	durationAlgorithm->output("duration").set(m_duration);
	
	durationAlgorithm->compute();
	delete durationAlgorithm;
	
	/*
	 * RhythmExtractor2013
	 */
	essentia::standard::Algorithm* rhythmExtractorAlgorithm = factory.create(
		"RhythmExtractor2013",
		"method", "multifeature"
	);
	
	// <--
	rhythmExtractorAlgorithm->input("signal").set(*m_audioBuffer);
	
	// -->
	essentia::Real bpm;
	essentia::Real confidence;
	std::vector<essentia::Real> estimates;
	std::vector<essentia::Real> bpmIntervals;
	rhythmExtractorAlgorithm->output("bpm").set(bpm);
	rhythmExtractorAlgorithm->output("ticks").set(m_ticks);
	rhythmExtractorAlgorithm->output("confidence").set(confidence);
	rhythmExtractorAlgorithm->output("estimates").set(estimates);
	rhythmExtractorAlgorithm->output("bpmIntervals").set(bpmIntervals);
	
	// compute
	rhythmExtractorAlgorithm->compute();
	delete rhythmExtractorAlgorithm;
	
	/*
	 * FrameCutter
	 */
	m_frameCutterAlgorithm = factory.create(
		"FrameCutter",
		"hopSize", 2048,
		"frameSize", 4096
	);
	m_frame = new std::vector<essentia::Real>();
	
	// <--
	m_frameCutterAlgorithm->input("signal").set(*m_audioBuffer);
	
	// -->
	m_frameCutterAlgorithm->output("frame").set(*m_frame);
	
	/*
	 * Windowing
	 */
	m_windowingAlgorithm = factory.create(
		"Windowing",
		"type", "blackmanharris62"
	);
	m_windowedFrame = new std::vector<essentia::Real>();
	
	// <--
	m_windowingAlgorithm->input("frame").set(*m_frame);
	
	// -->
	m_windowingAlgorithm->output("frame").set(*m_windowedFrame);
	
	/*
	 * Spectrum
	 */
	m_spectrumAlgorithm = factory.create(
		"Spectrum",
		"size", 4096
	);
	m_spectrumBuffer = new std::vector<essentia::Real>();
	
	// <--
	m_spectrumAlgorithm->input("frame").set(*m_windowedFrame);
	
	// -->
	m_spectrumAlgorithm->output("spectrum").set(*m_spectrumBuffer);
	
	/*
	 * Loudness
	 */
	m_loudnessAlgorithm = factory.create(
		"Loudness"
	);
	
	// <--
	m_loudnessAlgorithm->input("signal").set(*m_windowedFrame);
	
	// -->
	m_loudnessAlgorithm->output("loudness").set(m_loudness);
	
	/*
	 * StrongPeak
	 */
	m_strongPeakAlgorithm = factory.create(
		"StrongPeak"
	);
	
	// <--
	m_strongPeakAlgorithm->input("spectrum").set(*m_spectrumBuffer);
	
	// -->
	m_strongPeakAlgorithm->output("strongPeak").set(m_strongPeakValue);
}

void AudioAnalyzer::analyze()
{
	while (true)
	{
		m_frameCutterAlgorithm->compute();
		
		if (!m_frame->size())
			break;
			
		if (essentia::isSilent(*m_frame))
			continue;
		
		m_windowingAlgorithm->compute();
		m_spectrumAlgorithm->compute();
		m_loudnessAlgorithm->compute();
		m_strongPeakAlgorithm->compute();
		m_spectrums.push_back(Spectrum(*m_spectrumBuffer, m_strongPeakValue, m_loudness));
	}
}

void AudioAnalyzer::computeAverageLoudness()
{
	m_averageLoudness = 0.0f;
	for (std::vector<Spectrum>::iterator it = m_spectrums.begin(); it != m_spectrums.end(); it++)
		m_averageLoudness += it->getLoudness();
		
	m_averageLoudness /= m_spectrums.size();
}

void AudioAnalyzer::freeAlgorithms()
{
	// algorithms' data
	delete m_audioBuffer;
	delete m_frame;
	delete m_windowedFrame;
	delete m_spectrumBuffer;
	
	// algorithms
	delete m_frameCutterAlgorithm;
	delete m_windowingAlgorithm;
	delete m_spectrumAlgorithm;
	delete m_strongPeakAlgorithm;
}

void AudioAnalyzer::getSpectrum(float time, Spectrum** spectrum) const
{
	unsigned int index = floor(time / m_duration * m_spectrums.size());
	
	if (index < m_spectrums.size())
		*spectrum = (Spectrum*) &m_spectrums[index];
	
	else
		*spectrum = NULL;
}

} // game


