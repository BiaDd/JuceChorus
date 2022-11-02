/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DanDP04DooDooChorusAudioProcessorEditor::DanDP04DooDooChorusAudioProcessorEditor (DanDP04DooDooChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(585, 300);
}

DanDP04DooDooChorusAudioProcessorEditor::~DanDP04DooDooChorusAudioProcessorEditor()
{
}

//==============================================================================
void DanDP04DooDooChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    // Setup your sliders and other gui components - - - -
    auto& params = processor.getParameters();
    AudioParameterFloat* timeParam = (AudioParameterFloat*)params.getUnchecked(0);
    AudioParameterFloat* feedbackParam = (AudioParameterFloat*)params.getUnchecked(1);
    AudioParameterFloat* mixParam = (AudioParameterFloat*)params.getUnchecked(2);

    // Seconds slider
    mDelaySecSlider.setBounds(50, 50, 100, 160);
    mDelaySecSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDelaySecSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    mDelaySecSlider.setRange(timeParam->range.start, timeParam->range.end);
    mDelaySecSlider.setValue(*timeParam);
    mDelaySecSlider.addListener(this);
    addAndMakeVisible(mDelaySecSlider);

    mDelaySecSlider.setTextValueSuffix(" ms");
    addAndMakeVisible(mDelaySecLabel);
    mDelaySecLabel.setText("Time (Milliseconds)", juce::dontSendNotification);
    mDelaySecLabel.attachToComponent(&mDelaySecSlider, false);


    // feedback slider
    mFeedbackSlider.setBounds(250, 50, 100, 160);
    mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mFeedbackSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    mFeedbackSlider.setRange(feedbackParam->range.start, feedbackParam->range.end);
    mFeedbackSlider.setValue(*feedbackParam);
    mFeedbackSlider.addListener(this);
    addAndMakeVisible(mFeedbackSlider);

    //mFeedbackSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(mFeedbackSliderLabel);
    mFeedbackSliderLabel.setText("Feedback", juce::dontSendNotification);
    mFeedbackSliderLabel.attachToComponent(&mFeedbackSlider, false);


    // Wet dry slider

    mWetDrySlider.setBounds(450, 50, 100, 160);
    mWetDrySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mWetDrySlider.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    mWetDrySlider.setRange(mixParam->range.start, mixParam->range.end);
    mWetDrySlider.setValue(*mixParam);
    mWetDrySlider.addListener(this);
    addAndMakeVisible(mWetDrySlider);

    addAndMakeVisible(mWetDrySliderLabel);
    mWetDrySliderLabel.setText("Wet/Dry Mix", juce::dontSendNotification);
    mWetDrySliderLabel.attachToComponent(&mWetDrySlider, false);


}

void DanDP04DooDooChorusAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    auto& params = processor.getParameters();

    if (slider == &mDelaySecSlider)
    {
        juce::AudioParameterFloat* time = (juce::AudioParameterFloat*)params.getUnchecked(0);
        int t = int(mDelaySecSlider.getValue());
        *time = t;
        // DBG("Time Slider Changed");
    }
    else if (slider == &mFeedbackSlider)
    {
        juce::AudioParameterFloat* feedback = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *feedback = mFeedbackSlider.getValue();
        // DBG("Feedback Slider Changed");
    }
    else if (slider == &mWetDrySlider) {
        juce::AudioParameterFloat* wetDry = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *wetDry = mWetDrySlider.getValue();
        // DBG("Wet Dry Slider Changed");
    }
}

void DanDP04DooDooChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
