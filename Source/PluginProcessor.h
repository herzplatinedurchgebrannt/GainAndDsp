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
#include "EqProcessor.h"
#include "IRProcessor.h"

//==============================================================================
/**
*/
class TheGrillerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TheGrillerAudioProcessor();
    ~TheGrillerAudioProcessor() override;

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

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highPassFilter;

    DistortionAudioProcessor distortionProcessor;
    DynamicWaveshaper dynamicWaveshaper;
    EqProcessor eqProcessor;
    IRProcessor irProcessor;
    
    float eqBassOldValue = 0.f;
    float eqMidOldValue = 0.f;
    float eqHighOldValue = 0.f;
    int cabBoxSelectOldValue = 1;

    SingleEqBandProcessor toneControlEqProcessor;
    Gain<float> outputLevelProcessor;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheGrillerAudioProcessor)
};
