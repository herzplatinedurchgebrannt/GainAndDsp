#include "DynamicWaveshaper.h"

DynamicWaveshaper::~DynamicWaveshaper()
{
}

void DynamicWaveshaper::prepare(const juce::dsp::ProcessSpec &spec) noexcept
{
    sampleRate = spec.sampleRate;
    maximumBlockSize = spec.maximumBlockSize;
    numChannels = spec.numChannels;
    envOutputBuffer = std::make_shared<juce::AudioBuffer<float>>(numChannels, maximumBlockSize);
    envOutputBlock = std::make_shared<juce::dsp::AudioBlock<float>>(*envOutputBuffer);
    envelopeFollower.prepare(spec);
    parametricWaveshaper.prepare(spec);
    setAttackTime(0.15f);
    setReleaseTime(0.10f);
}

void DynamicWaveshaper::setAttackTime(float attackTime)
{
    envelopeFollower.setAttackTime(attackTime);
}

void DynamicWaveshaper::setReleaseTime(float releaseTime)
{
    envelopeFollower.setReleaseTime(releaseTime);
}

void DynamicWaveshaper::reset() noexcept
{
}

void DynamicWaveshaper::process(const juce::dsp::ProcessContextReplacing<float> &context) noexcept
{
    juce::dsp::ProcessContextNonReplacing<float> envContext(context.getInputBlock(), *envOutputBlock);
    envelopeFollower.process(envContext);
    parametricWaveshaper.processWithSidechain(context, envOutputBuffer);
}
