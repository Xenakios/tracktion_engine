/*
    ,--.                     ,--.     ,--.  ,--.
  ,-'  '-.,--.--.,--,--.,---.|  |,-.,-'  '-.`--' ,---. ,--,--,      Copyright 2018
  '-.  .-'|  .--' ,-.  | .--'|     /'-.  .-',--.| .-. ||      \   Tracktion Software
    |  |  |  |  \ '-'  \ `--.|  \  \  |  |  |  |' '-' '|  ||  |       Corporation
    `---' `--'   `--`--'`---'`--'`--' `---' `--' `---' `--''--'    www.tracktion.com

    Tracktion Engine uses a GPL/commercial licence - see LICENCE.md for details.
*/

namespace tracktion_engine
{

/** */
class TimeStretcher
{
public:
    TimeStretcher();
    ~TimeStretcher();

    enum Mode
    {
        disabled = 0,
        elastiqueTransient = 1, // defunct
        elastiqueTonal = 2,     // defunct
        soundtouchNormal = 3,
        soundtouchBetter = 4,
        melodyne = 5,
        elastiquePro = 6,
        elastiqueEfficient = 7,
        elastiqueMobile = 8,
        elastiqueMonophonic = 9,
		rubberband = 100,
       #if TRACKTION_ENABLE_TIMESTRETCH_ELASTIQUE
        defaultMode = elastiquePro
       #elif TRACKTION_ENABLE_TIMESTRETCH_SOUNDTOUCH
		defaultMode = soundtouchBetter
	   #elif TRACKTION_ENABLE_TIMESTRETCH_RUBBERBAND
		defaultMode = rubberband
	   #else
        defaultMode = disabled
       #endif
    };

    struct StretcherAdditionalOptions
    {
        StretcherAdditionalOptions() {}
        StretcherAdditionalOptions (const juce::String& string);

        juce::String toString() const;
        bool operator== (const StretcherAdditionalOptions& other) const;
        bool operator!= (const StretcherAdditionalOptions& other) const;

        bool midSideStereo = false;
        bool syncTimeStrPitchShft = false;
        bool preserveFormants = false;
        int envelopeOrder = 64;
		Mode stretcherMode = elastiquePro;
		int64_t rubberBandOptions = 0;
    };

    static Mode checkModeIsAvailable (Mode);
    static juce::StringArray getPossibleModes (Engine&, bool excludeMelodyne);
    static Mode getModeFromName (Engine&, const juce::String& name);
    static juce::String getNameOfMode (Mode mode);
    static bool isMelodyne (Mode mode);

    void initialise (double sourceSampleRate, int samplesPerBlock,
                     int numChannels, Mode mode, StretcherAdditionalOptions options, bool realtime);

    bool isInitialised() const;
    void reset();
    bool setSpeedAndPitch (float speedRatio, float semitones);

    static bool canProcessFor (Mode);

    int getFramesNeeded() const;
    int getMaxFramesNeeded() const;
    void processData (const float* const* inChannels, int numSamples, float* const* outChannels);
    void processData (AudioFifo& inFifo, int numSamples, AudioFifo& outFifo);
    void flush (float* const* outChannels);

	struct Stretcher
	{
		virtual ~Stretcher() {}

		virtual bool isOk() const = 0;
		virtual void reset() = 0;
		virtual bool setSpeedAndPitch(float speedRatio, float semitonesUp) = 0;
		virtual int getFramesNeeded() const = 0;
		virtual int getMaxFramesNeeded() const = 0;
		virtual void processData(const float* const* inChannels, int numSamples, float* const* outChannels) = 0;
		virtual void flush(float* const* outChannels) = 0;
	};


private:
    std::unique_ptr<Stretcher> stretcher;
    int samplesPerBlockRequested = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeStretcher)
};



} // namespace tracktion_engine

tracktion_engine::TimeStretcher::Stretcher* createRubber(double sr, int samplePerBlock, int numchans);

namespace juce
{
    template <>
    struct VariantConverter<tracktion_engine::TimeStretcher::Mode>
    {
        static tracktion_engine::TimeStretcher::Mode fromVar (const var& v)   { return (tracktion_engine::TimeStretcher::Mode) static_cast<int> (v); }
        static var toVar (tracktion_engine::TimeStretcher::Mode v)            { return static_cast<int> (v); }
    };

    template <>
    struct VariantConverter<tracktion_engine::TimeStretcher::StretcherAdditionalOptions>
    {
        static tracktion_engine::TimeStretcher::StretcherAdditionalOptions fromVar (const var& v) { return tracktion_engine::TimeStretcher::StretcherAdditionalOptions (v.toString()); }
        static var toVar (const tracktion_engine::TimeStretcher::StretcherAdditionalOptions& v)   { return v.toString(); }
    };
}
