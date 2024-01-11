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
    _sampleRate = spec.sampleRate;
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

    output *= _drive * _range;

    output = (((((2.f / juce::float_Pi) * atan(output)) * _blend) + (cleanSig * (1.f - _blend))) / 2.f) * _volume;

    return output;
}

void DistortionAudioProcessor::setValues(float drive, float range, float blend, float volume)
{
    _drive = drive;
    _range = range;
    _blend = blend;
    _volume = volume;
}