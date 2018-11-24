/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

  name:             PitchAndTimeDemo
  version:          0.0.1
  vendor:           Tracktion
  website:          www.tracktion.com
  description:      This example shows how to load an audio clip and adjust its speed and pitch so you can play along with it in a different key or tempo.

  dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats, juce_audio_processors, juce_audio_utils,
                    juce_core, juce_data_structures, juce_dsp, juce_events, juce_graphics,
                    juce_gui_basics, juce_gui_extra, tracktion_engine
  exporters:        linux_make, vs2017, xcode_iphone, xcode_mac

  moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1, JUCE_PLUGINHOST_VST3=1, JUCE_PLUGINHOST_AU=1, TRACKTION_ENABLE_TIMESTRETCH_SOUNDTOUCH=1

  type:             Component
  mainClass:        PitchAndTimeComponent

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "common/Utilities.h"


//==============================================================================
class PitchAndTimeComponent   : public Component,
                                private ChangeListener
{
public:
    //==============================================================================
    PitchAndTimeComponent()
    {
        transport.addChangeListener (this);
        updatePlayButtonText();

        Helpers::addAndMakeVisible (*this,
                                    { &settingsButton, &playPauseButton, &loadFileButton, &thumbnail,
                                      &rootNoteEditor, &rootTempoEditor, &keySlider, &tempoSlider,& reverseButton, 
							          &timePitchModeCombo, &volumeSlider });
		auto modes = te::TimeStretcher::getPossibleModes(engine, true);
		for (int i=0;i<modes.size();++i)
		{
			auto modetext = modes[i];
			timePitchModeCombo.addItem(modetext, i + 1);
		}
		
		volumeSlider.setRange(0.0, 1.0);
		volumeSlider.onValueChange = [this]() { edit.setMasterVolumeSliderPos(volumeSlider.getValue()); };
		volumeSlider.setValue(0.5);

		timePitchModeCombo.setSelectedId(3, dontSendNotification);
		timePitchModeCombo.onChange = [this]() { updateTempoAndKey(); };
		settingsButton.onClick  = [this] { EngineHelpers::showAudioDeviceSettings (engine); };
        playPauseButton.onClick = [this] { EngineHelpers::togglePlay (edit); };
        loadFileButton.onClick  = [this] { EngineHelpers::browseForAudioFile (engine, [this] (const File& f) { setFile (f); }); };

        rootNoteEditor.onFocusLost = [this] { updateTempoAndKey(); };
        rootTempoEditor.onFocusLost = [this] { updateTempoAndKey(); };

        keySlider.setRange (-12.0, 12.0, 1.0);
        keySlider.onDragEnd = [this] { updateTempoAndKey(); };
        tempoSlider.setRange (30.0, 220.0, 0.1);
        tempoSlider.onDragEnd = [this] { updateTempoAndKey(); };

        keySlider.textFromValueFunction = [this] (double v)
                                          {
                                              const int numSemitones = roundToInt (v);
                                              const auto semitonesText = (numSemitones > 0 ? "+" : "") + String (numSemitones);

                                              if (auto clip = getClip())
                                              {
                                                  const auto audioFileInfo = te::AudioFile (clip->getSourceFileReference().getFile()).getInfo();
                                                  const auto rootNote = audioFileInfo.loopInfo.getRootNote();

                                                  if (rootNote <= 0)
                                                      return semitonesText;

                                                  return semitonesText + " (" + MidiMessage::getMidiNoteName (rootNote + numSemitones, true, false, 3) + ")";
                                              }

                                              return semitonesText;
                                          };
		
		reverseButton.setButtonText("Reverse");
		reverseButton.onClick = [this]() { updateTempoAndKey(); };

		setSize(900, 500);
    }

    ~PitchAndTimeComponent()
    {
		thumbnail.setFile({});
		edit.getTempDirectory (false).deleteRecursively();
	}

    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto r = getLocalBounds();

        {
            auto topR = r.removeFromTop (30);
            settingsButton.setBounds (topR.removeFromLeft (topR.getWidth() / 3).reduced (2));
            playPauseButton.setBounds (topR.removeFromLeft (topR.getWidth() / 2).reduced (2));
            loadFileButton.setBounds (topR.reduced (2));
        }

        {
			rootNoteEditor.setBounds(1, getHeight() - 90, getWidth() / 2 - 2, 29);
			keySlider.setBounds(rootNoteEditor.getRight() + 2, rootNoteEditor.getY(), getWidth() / 2 - 2, 29);
			rootTempoEditor.setBounds(1, rootNoteEditor.getBottom() + 1, getWidth() / 2 - 2, 29);
			tempoSlider.setBounds(rootTempoEditor.getRight() + 2, rootNoteEditor.getBottom() + 1, getWidth() / 2 - 2, 29);
			reverseButton.setBounds(1, tempoSlider.getBottom() + 1, 100, 29);
			timePitchModeCombo.setBounds(reverseButton.getRight() + 2, tempoSlider.getBottom() + 1, 200, 29);
			volumeSlider.setBounds(timePitchModeCombo.getRight()+2, tempoSlider.getBottom() + 1, 200, 29);
        }

		thumbnail.setBounds(0, settingsButton.getBottom() + 2, getWidth(), 
			getHeight()-(90 + settingsButton.getHeight()) - 10);
    }

private:
    //==============================================================================
    te::Engine engine { ProjectInfo::projectName };
    te::Edit edit { engine, te::createEmptyEdit(), te::Edit::forEditing, nullptr, 0 };
    te::TransportControl& transport { edit.getTransport() };

    FileChooser audioFileChooser { "Please select an audio file to load...",
                                   engine.getPropertyStorage().getDefaultLoadSaveDirectory ("pitchAndTimeExample"),
                                   engine.getAudioFileFormatManager().readFormatManager.getWildcardForAllFormats() };

    TextButton settingsButton { "Settings" }, playPauseButton { "Play" }, loadFileButton { "Load file" };
    Thumbnail thumbnail { transport };
    TextEditor rootNoteEditor, rootTempoEditor;
    Slider keySlider, tempoSlider, volumeSlider;
	ToggleButton reverseButton;
	ComboBox timePitchModeCombo;
    //==============================================================================
    te::WaveAudioClip::Ptr getClip()
    {
        if (auto track = edit.getOrInsertAudioTrackAt (0))
            if (auto clip = dynamic_cast<te::WaveAudioClip*> (track->getClips()[0]))
                return *clip;

        return {};
    }

    File getSourceFile()
    {
        if (auto clip = getClip())
            return clip->getSourceFileReference().getFile();

        return {};
    }

    void setFile (const File& f)
    {
        keySlider.setValue (0.0, dontSendNotification);
        tempoSlider.setValue (120.0, dontSendNotification);

        if (auto clip = EngineHelpers::loadAudioFileAsClip (edit, f))
        {
            // Disable auto tempo and pitch, we'll handle these manually
			
			clip->setAutoTempo (false);
            clip->setAutoPitch (false);
            clip->setTimeStretchMode (te::TimeStretcher::rubberband);
			clip->setIsReversed(reverseButton.getToggleState());
			clip->setGainDB(-12.0f);
            thumbnail.setFile (EngineHelpers::loopAroundClip (*clip)->getPlaybackFile());

            const auto audioFileInfo = te::AudioFile (f).getInfo();
            const auto loopInfo = audioFileInfo.loopInfo;
            auto tempo = loopInfo.getBpm (audioFileInfo);
			if (tempo < 1.0)
				tempo = 120.0;
            rootNoteEditor.setText (TRANS("Root Key: ") + Helpers::getStringOrDefault (MidiMessage::getMidiNoteName (loopInfo.getRootNote(), true, false, 3), "Unknown"), false);
            rootTempoEditor.setText (TRANS("Root Tempo: ") + Helpers::getStringOrDefault (String (tempo, 2), "Unknown"), false);

            keySlider.setValue (0.0, dontSendNotification);
            keySlider.updateText();

            if (tempo > 0)
                tempoSlider.setValue (tempo, dontSendNotification);
        }
        else
        {
            thumbnail.setFile ({});
            rootNoteEditor.setText (TRANS("Root Key: Unknown"));
            rootTempoEditor.setText (TRANS("Root Tempo:  Unknown"));
        }
    }

    void updateTempoAndKey()
    {
        if (auto clip = getClip())
        {
            auto f = getSourceFile();
            const auto audioFileInfo = te::AudioFile (f).getInfo();
            const double baseTempo = rootTempoEditor.getText().retainCharacters ("+-.0123456789").getDoubleValue();
			auto modename = te::TimeStretcher::getNameOfMode(clip->getTimeStretchMode());
			if (modename != timePitchModeCombo.getText())
			{
				clip->setTimeStretchMode(te::TimeStretcher::getModeFromName(engine,timePitchModeCombo.getText()));
			}
			// First update the tempo based on the ratio between the root tempo and tempo slider value
            if (baseTempo > 0.0)
            {
                const double ratio = tempoSlider.getValue() / baseTempo;
                clip->setSpeedRatio (ratio);
                clip->setLength (audioFileInfo.getLengthInSeconds() / clip->getSpeedRatio(), true);
            }

			clip->setGainDB(-12.0f);

            // Then update the pitch change based on the slider value
            clip->setPitchChange (float (keySlider.getValue()));

			clip->setIsReversed(reverseButton.getToggleState());
			Logger::writeToLog("Needs render " + String(clip->getHash()));
			
			// Update the thumbnail to show the proxy render progress
			
			
			thumbnail.setFile(EngineHelpers::loopAroundClip(*clip)->getPlaybackFile());
            Logger::writeToLog(edit.state.toXmlString());
        }
    }

    void updatePlayButtonText()
    {
        playPauseButton.setButtonText (transport.isPlaying() ? "Pause" : "Play");
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        updatePlayButtonText();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchAndTimeComponent)
};
