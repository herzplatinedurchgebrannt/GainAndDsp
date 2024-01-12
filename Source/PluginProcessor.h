/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DynamicWaveshaper.h"
#include "SingleEqBandProcessor.h"
#include "DistortionProcessor.h"
#include "Eq4Band.h"
#include "IRProcessor.h"

//==============================================================================
/**
*/
class GainAndDspAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GainAndDspAudioProcessor();
    ~GainAndDspAudioProcessor() override;

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

    void updateParams();

    juce::AudioProcessorValueTreeState valueTree;

    float lastSampleRate;

private:

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;

    DistortionAudioProcessor distortionProcessor;
    DynamicWaveshaper dynamicWaveshaper;

    SingleEqBandProcessor toneControlEqProcessor;

    Eq4Band eqProcessor;
    IRProcessor irProcessor;

    

    //bool cabSelectValueOld = true;
    int cabBoxSelectOldValue = 1;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainAndDspAudioProcessor)
};
