/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.1/BiQuad.h"
#include "StkLite-4.6.1/Delay.h"
#include "Mu45LFO/Mu45LFO.h"
#define MAXDELAYMSEC 1000.0

//==============================================================================
/**
*/
class DanDP04DooDooChorusAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DanDP04DooDooChorusAudioProcessor();
    ~DanDP04DooDooChorusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    void calcAlgorithmParams();
    // convert MIDI Note Number to Hz
    float calcMiditoHz(float midiNote) { return 440.0 * powf(2, (midiNote - 60.0) / 12.0); }
    // // convert Hz to MIDI Note Number
    float calcHzToMidi(float Hz) { return 12 * log2f(Hz / 440.0) + 60.0; }
    //int calcMsecToSamps(float msec);

    // User Parameters
    juce::AudioParameterFloat* mWetDryMixParam;
    juce::AudioParameterFloat* mDelayMsecParam;

    // LFO stuff
    juce::AudioParameterFloat* mCenterFreqParam;
    juce::AudioParameterFloat* mLfoDepthParam;
    juce::AudioParameterFloat* mLfoSpeedParam;

    juce::AudioParameterFloat* mFeedbackDbParam;

    // Algorithm Parameters
    float mFeedbackGainLin;
    float mWetGain, mDryGain;

    // Member objects we need
    stk::Delay mDelay1L, mDelay1R; // The delay objects
    float mFs; // Sampling rate
    // Algorithm Parameters for LFO
    float mFilterQ = 2;
    float mMinFc;
    float mMaxFc;

    // LFO things
    Mu45LFO mLFO;
    stk::BiQuad mFilterL, mFilterR;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DanDP04DooDooChorusAudioProcessor)
};
