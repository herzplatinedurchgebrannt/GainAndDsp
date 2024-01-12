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
    processWithSidechain(context, envOutputBuffer);
}

float DynamicWaveshaper::processSample(
    int channel, int sample, float x, std::shared_ptr<juce::AudioBuffer<float>> sidechainBuffer)
{
    float parameter = sidechainBuffer->getSample(channel, sample);
    return (x * (abs(x) + parameter) / (x * x + (parameter - 1) * abs(x) + 1)) * 0.7;
}

// Yeah, there's some duplication in here but creating some weird abstraction is not worth it
void DynamicWaveshaper::processWithSidechain(
    const juce::dsp::ProcessContextReplacing<float>& context, std::shared_ptr<juce::AudioBuffer<float>> sidechainBuffer) noexcept
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
            dst[sample] = juce::jlimit(-1.f, 1.f, processSample(channel, sample, src[sample], sidechainBuffer));
        }
    }
}