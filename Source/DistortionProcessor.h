/*
  ==============================================================================

    DistortionProcessor.h
    Created: 10 Jan 2024 10:59:26am
    Author:  alex

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DistortionAudioProcessor 
{
public:
    DistortionAudioProcessor();
    ~DistortionAudioProcessor();


    void prepare(const::juce::dsp::ProcessSpec& spec) noexcept;
    void process(const juce::dsp::ProcessContextReplacing<float>& context) noexcept;
    float processSample(float x);
    void setValues(float drive, float range, float blend, float volume);

private:

    float sampleRate = 0;
    float drive = 0.5f;
    float range = 1500.0f;
    float blend = 0.5f;
    float volume = 1.0f;
};


