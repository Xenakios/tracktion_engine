/*
    ,--.                     ,--.     ,--.  ,--.
  ,-'  '-.,--.--.,--,--.,---.|  |,-.,-'  '-.`--' ,---. ,--,--,      Copyright 2018
  '-.  .-'|  .--' ,-.  | .--'|     /'-.  .-',--.| .-. ||      \   Tracktion Software
    |  |  |  |  \ '-'  \ `--.|  \  \  |  |  |  |' '-' '|  ||  |       Corporation
    `---' `--'   `--`--'`---'`--'`--' `---' `--' `---' `--''--'    www.tracktion.com

    Tracktion Engine uses a GPL/commercial licence - see LICENCE.md for details.
*/

#if ! JUCE_PROJUCER_LIVE_BUILD

#include "tracktion_engine.h"

#if TRACKTION_ENABLE_TIMESTRETCH_ELASTIQUE
 // If you get a build error here you'll need to add the Elastique SDK to your header search path!
 #include "elastique_pro/incl/elastiqueProV3API.h"
#endif

using namespace juce;

#ifdef __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wunused-parameter"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wmacro-redefined"
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wunused"
#endif

#ifdef JUCE_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4005 4189 4189 4267 4702 4458 4100)
#endif

#include "timestretch/tracktion_TimeStretch.cpp"

namespace tracktion_engine
{
    #include "3rd_party/soundtouch/source/SoundTouch/BPMDetect.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/PeakFinder.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/FIFOSampleBuffer.cpp"

   #if TRACKTION_ENABLE_TIMESTRETCH_SOUNDTOUCH
    #include "3rd_party/soundtouch/source/SoundTouch/AAFilter.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/cpu_detect_x86.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/FIRFilter.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/InterpolateCubic.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/InterpolateLinear.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/InterpolateShannon.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/mmx_optimized.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/RateTransposer.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/SoundTouch.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/sse_optimized.cpp"
    #include "3rd_party/soundtouch/source/SoundTouch/TDStretch.cpp"
   #endif
   
}

#ifdef JUCE_MSVC
 #pragma warning (pop)
#endif

#ifdef __clang__
 #pragma clang diagnostic pop
#endif

#if TRACKTION_ENABLE_TIMESTRETCH_RUBBERBAND
 #ifdef JUCE_MSVC
  #define __MSVC__
 #endif
 #define USE_KISSFFT
 #define USE_SPEEX
 #ifndef __MSVC__
  #define USE_PTHREADS
 #endif
 #include "3rd_party/rubberband/rbstretch.cpp"
 #include "3rd_party/rubberband/src/RubberBandStretcher.cpp"
 #include "3rd_party/rubberband/src/StretchCalculator.cpp"
 #include "3rd_party/rubberband/src/StretcherChannelData.cpp"
 #include "3rd_party/rubberband/src/StretcherImpl.cpp"
 #include "3rd_party/rubberband/src/StretcherProcess.cpp"
 #include "3rd_party/rubberband/src/speex/resample.c"
 #include "3rd_party/rubberband/src/kissfft/kiss_fft.c"
 #include "3rd_party/rubberband/src/kissfft/kiss_fftr.c"
 #include "3rd_party/rubberband/src/audiocurves/CompoundAudioCurve.cpp"
 #include "3rd_party/rubberband/src/audiocurves/ConstantAudioCurve.cpp"
 #include "3rd_party/rubberband/src/audiocurves/HighFrequencyAudioCurve.cpp"
 #include "3rd_party/rubberband/src/audiocurves/PercussiveAudioCurve.cpp"
 #include "3rd_party/rubberband/src/audiocurves/SilentAudioCurve.cpp"
 #include "3rd_party/rubberband/src/audiocurves/SpectralDifferenceAudioCurve.cpp"
 #include "3rd_party/rubberband/src/base/Profiler.cpp"
 #include "3rd_party/rubberband/src/dsp/AudioCurveCalculator.cpp"
 #include "3rd_party/rubberband/src/dsp/FFT.cpp"
 #include "3rd_party/rubberband/src/dsp/Resampler.cpp"
 #include "3rd_party/rubberband/src/system/Allocators.cpp"
 #include "3rd_party/rubberband/src/system/sysutils.cpp"
 #include "3rd_party/rubberband/src/system/Thread.cpp"
 #include "3rd_party/rubberband/src/system/VectorOpsComplex.cpp"
 
#endif
#endif
