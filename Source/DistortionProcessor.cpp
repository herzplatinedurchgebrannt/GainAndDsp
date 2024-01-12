/*
  ==============================================================================

    DistortionProcessor.cpp
    Created: 10 Jan 2024 10:59:26am
    Author:  alex

  ==============================================================================
*/

#include "DistortionProcessor.h"

DistortionAudioProcessor::DistortionAudioProcessor()
{

}

DistortionAudioProcessor::~DistortionAudioProcessor() 
{

}

void DistortionAudioProcessor::prepare(const::juce::dsp::ProcessSpec& spec) noexcept 
{
    sampleRate = spec.sampleRate;
}

void DistortionAudioProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) noexcept 
{
    auto&& inBlock = context.getInputBlock();
    auto&& outBlock = context.getOutputBlock();

    jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
    jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

    auto len = inBlock.getNumSamples();
    auto numChannels = inBlock.getNumChannels();

    if (context.isBypassed)
    {
        if (context.usesSeparateInputAndOutputBlocks())
            outBlock.copyFrom(inBlock);
        return;
    }

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* src = inBlock.getChannelPointer(channel);
        auto* dst = outBlock.getChannelPointer(channel);
        for (int sample = 0; sample < len; ++sample)
        {
            dst[sample] = juce::jlimit(-1.f, 1.f, processSample(src[sample]));
        }
    }
}

float DistortionAudioProcessor::processSample(float sample)
{
    float output = sample;
    float cleanSig = sample;

    output *= drive * range;

    output = (((((2.f / juce::float_Pi) * atan(output)) * blend) + (cleanSig * (1.f - blend))) / 2.f) * volume;

    return output;
}

void DistortionAudioProcessor::setValues(float driveNewValue, float rangeNewValue, float blendNewValue, float volumeNewValue)
{
    drive = driveNewValue;
    range = rangeNewValue;
    blend = blendNewValue;
    volume = volumeNewValue;
}