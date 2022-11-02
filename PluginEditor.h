/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce;

//==============================================================================
/**
*/
class DanDP04DooDooChorusAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                 public juce::Slider::Listener
{
public:
    DanDP04DooDooChorusAudioProcessorEditor (DanDP04DooDooChorusAudioProcessor&);
    ~DanDP04DooDooChorusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DanDP04DooDooChorusAudioProcessor& audioProcessor;

    juce::Slider mDelaySecSlider;
    juce::Slider mFeedbackSlider;
    juce::Slider mWetDrySlider;

    juce::Label mDelaySecLabel;
    juce::Label mFeedbackSliderLabel;
    juce::Label mWetDrySliderLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DanDP04DooDooChorusAudioProcessorEditor)
};
