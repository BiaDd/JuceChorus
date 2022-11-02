/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DanDP04DooDooChorusAudioProcessor::DanDP04DooDooChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(mDelayMsecParam = new juce::AudioParameterFloat("DelaySec", // parameterID,
        "DelaySec", // parameterName,
        500.0f, // minValue,
        5000.0f, // maxValue,
        1000.0f)); // defaultValue

    addParameter(mFeedbackDbParam = new juce::AudioParameterFloat("Feedback", // parameterID,
        "Feedback", // parameterName,
        -60.0f, // minValue,
        0.0f, // maxValue,
        -30.f)); // defaultValue

    addParameter(mWetDryMixParam = new juce::AudioParameterFloat("WetDryMix", // parameterID,
        "WetDryMix", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        20.0f)); // defaultValue


    // LFO things
    addParameter(mCenterFreqParam = new juce::AudioParameterFloat("LFOCenterFreq", // parameterID,
        "LFOCenterFreq", // parameterName,
        500.0f, // minValue,
        5000.0f, // maxValue,
        1000.0f)); // defaultValue

    
    // how strong delay is coming through
    // maybe have it as the second knob
    addParameter(mLfoDepthParam = new juce::AudioParameterFloat("LFODepth", // parameterID,
        "LFODepth", // parameterName,
        -60.0f, // minValue,
        0.0f, // maxValue,
        -30.f)); // defaultValue

    // maybe on let change LFO speed (rate knob)
    addParameter(mLfoSpeedParam = new juce::AudioParameterFloat("LFOSpeed", // parameterID,
        "LFOSpeed", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        20.0f)); // defaultValue



}

DanDP04DooDooChorusAudioProcessor::~DanDP04DooDooChorusAudioProcessor()
{
}

//==============================================================================
const juce::String DanDP04DooDooChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DanDP04DooDooChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DanDP04DooDooChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DanDP04DooDooChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DanDP04DooDooChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DanDP04DooDooChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DanDP04DooDooChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DanDP04DooDooChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DanDP04DooDooChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void DanDP04DooDooChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DanDP04DooDooChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mFs = sampleRate;

    mMinFc = 10.0; // the min we would set out center freq to
    mMaxFc = mFs * 0.5 * 0.95; // the max we would set out center freq to

    // Initialize your Delay objects with maximum length
    int maxSamps = int(mFs * (MAXDELAYMSEC / 1000));
    mDelay1L.setMaximumDelay(maxSamps);
    mDelay1R.setMaximumDelay(maxSamps);


    mWetGain = 0.5;
    mDryGain = 0.5;
    mFeedbackGainLin = -0.30;
}

void DanDP04DooDooChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DanDP04DooDooChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// set the delays from the parameter
void DanDP04DooDooChorusAudioProcessor::calcAlgorithmParams()
{
    // set the wet & dry gains
    mWetGain = mWetDryMixParam->get() / 100.0;
    mDryGain = 1.0 - mWetGain;

    float delaySec = mDelayMsecParam->get();
    // set the delay times
    int samps = int(delaySec / 1000 * mFs);
    mDelay1L.setDelay(samps);
    mDelay1R.setDelay(samps);

    // set the feedback gain
    mFeedbackGainLin = powf(10.0, mFeedbackDbParam->get() * 0.05);

    // LFO stuff
    mLFO.setFreq(mLfoSpeedParam->get(), mFs);
}


void DanDP04DooDooChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Process each audio sample
    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    calcAlgorithmParams();

    float tempL, tempR;

    // LFO
    float lfoOut, centerMidi, centerFreq;
    float coeffs[5];

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

        for (int samp = 0; samp < buffer.getNumSamples(); samp++)
        {

            // LFO thing
            lfoOut = mLFO.tick(); // output of LFO varies between -1.0 and +1.0
            centerMidi = mCenterFreqParam->get() + mLfoDepthParam->get() * lfoOut;
            centerFreq = calcMiditoHz(centerMidi);

            // Ensure the center frequency is not out of range
            centerFreq = (centerFreq < mMinFc) ? mMinFc : centerFreq;
            centerFreq = (centerFreq > mMaxFc) ? mMaxFc : centerFreq;

            // Calc & then set the filter coeffs
            Mu45FilterCalc::calcCoeffsBPF(coeffs, centerFreq, mFilterQ, mFs);
            mFilterL.setCoefficients(coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]);
            mFilterR.setCoefficients(coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]);


            // --------------------------- Delay
            // get next out
            tempL = mDelay1L.nextOut();
            tempR = mDelay1R.nextOut();

            // get the output of the delay
            mDelay1L.tick(channelDataLeft[samp] + mFeedbackGainLin * tempL);
            mDelay1R.tick(channelDataRight[samp] + mFeedbackGainLin * tempR);

            // need to do LFO filter here? before multiplying everyting
            // Actually filter the audio
            channelDataLeft[samp] = mFilterL.tick(channelDataLeft[samp]);
            channelDataRight[samp] = mFilterR.tick(channelDataRight[samp]);


            channelDataLeft[samp] = mDryGain * channelDataLeft[samp] + mWetGain * tempL;
            channelDataRight[samp] = mDryGain * channelDataLeft[samp] + mWetGain * tempR;
        }
    }
}

//==============================================================================
bool DanDP04DooDooChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DanDP04DooDooChorusAudioProcessor::createEditor()
{
    return new DanDP04DooDooChorusAudioProcessorEditor (*this);
}

//==============================================================================
void DanDP04DooDooChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DanDP04DooDooChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DanDP04DooDooChorusAudioProcessor();
}
