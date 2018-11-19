#if TRACKTION_ENABLE_TIMESTRETCH_RUBBERBAND
#include <memory>


//#include "../JuceLibraryCode/JuceHeader.h"
//using namespace std;
#include "src/rubberband/RubberBandStretcher.h"
using namespace tracktion_engine;

struct RubberBandTimeStretcher : public TimeStretcher::Stretcher
{
	RubberBandTimeStretcher(double sourceSampleRate, int samplesPerBlock, int numChannels, TimeStretcher::Mode mode)
	{
		m_rb = std::make_unique<RubberBand::RubberBandStretcher>(sourceSampleRate, numChannels, 
			RubberBand::RubberBandStretcher::OptionProcessRealTime);
		m_max_samples = samplesPerBlock;
		m_rb->setMaxProcessSize(samplesPerBlock);
		
	}
	bool isOk() const override
	{
		return m_rb!=nullptr;
	}
	void reset() override
	{
		if (m_rb)
			m_rb->reset();
        m_proc_count = 0;
	}
	bool setSpeedAndPitch(float speedRatio, float semitonesUp) override
	{
		if (m_rb==nullptr)
			return false;
		m_rb->setPitchScale(pow(2.0, semitonesUp / 12.0));
		m_rb->setTimeRatio(speedRatio);
		return true;
	}
	int getFramesNeeded() const override
	{
		if (m_rb == nullptr)
			return 0;
		return m_max_samples;
	}
	int getMaxFramesNeeded() const override
	{
		if (m_rb == nullptr)
			return 0;
		return m_max_samples;
	}
	void processData(const float* const* inChannels, int numSamples, float* const* outChannels) override
	{
		if (m_rb == nullptr)
			return;
		m_rb->process(inChannels, numSamples, false);
		if (m_rb->available() > numSamples)
		{
			int r = m_rb->retrieve(outChannels, numSamples);
			jassert(r == numSamples);
		}
		else
		{
			for (int i = 0; i < numSamples; ++i)
				for (int j = 0; j < m_rb->getChannelCount(); ++j)
					outChannels[j][i] = 0.0f;
		}
	}
	void flush(float* const* /*outChannels*/) override
	{

	}
private:
	std::unique_ptr<RubberBand::RubberBandStretcher> m_rb;
	int m_max_samples = 0;
    int m_proc_count = 0;
};

TimeStretcher::Stretcher* createRubber(double sr, int samplesPerBlock, int numchans)
	{
		return new RubberBandTimeStretcher(sr, samplesPerBlock, numchans, TimeStretcher::rubberband);
	}

#endif
