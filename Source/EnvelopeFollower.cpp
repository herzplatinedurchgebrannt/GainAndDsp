#include "EnvelopeFollower.h"

EnvelopeFollower::EnvelopeFollower()
{
    mode = 2;
}

EnvelopeFollower::~EnvelopeFollower()
{
}

void EnvelopeFollower::setSampleRate(float newSampleRate)
{
    if (newSampleRate > 0.01f)
        sampleRate = newSampleRate;

    setAttackTime(attackTime);
    setReleaseTime(releaseTime);

    // coeffAttack  = exp( -1.0f / (sampleRate*attackTime)  );
    // coeffRelease = exp( -1.0f / (sampleRate*releaseTime) );
}

void EnvelopeFollower::setAttackTime(float newAttackTime)
{
    if (newAttackTime > 0.0f)
    {
        attackTime = newAttackTime;
        coeffAttack = exp(-1.0f / (sampleRate * attackTime));
    }
    else
    {
        attackTime = 0.0f;
        coeffAttack = 0.0f;
        ;
    }
}

void EnvelopeFollower::setReleaseTime(float newReleaseTime)
{
    if (newReleaseTime > 0.0f)
    {
        releaseTime = newReleaseTime;
        coeffRelease = exp(-1.0f / (sampleRate * releaseTime));
    }
    else
    {
        releaseTime = 0.0f;
        coeffRelease = 0.0f;
    }
}


void EnvelopeFollower::prepare(const juce::dsp::ProcessSpec &spec) noexcept
{
    setSampleRate(spec.sampleRate);
    reset();
}

// Yeah, there's some more duplication here. Sue me.
void EnvelopeFollower::process(const juce::dsp::ProcessContextNonReplacing<float> &context) noexcept
{
    auto &&inBlock = context.getInputBlock();
    auto &&outBlock = context.getOutputBlock();

    jassert(inBlock.getNumChannels() == outBlock.getNumChannels());

    // I don't necessarily need to know the exact sample count for the buffer I use here
    // (as long as it's long enough). If this were outputting samples for use as plugin output
    // I would be in trouble, but this is just the sidechain for another plugin.
    // BE AWARE of that, and refactor if you need this to behave like a regular processor.
    // jassert (inBlock.getNumSamples() == outBlock.getNumSamples());

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
        auto *src = inBlock.getChannelPointer(channel);
        auto *dst = outBlock.getChannelPointer(channel);
        for (int sample = 0; sample < len; ++sample)
        {
            dst[sample] = juce::jlimit(-1.f, 1.f, getSample(src[sample]));
        }
    }
}

void EnvelopeFollower::setMode(int Mode)
{
    if (Mode >= MEAN_ABS && Mode < NUM_MODES)
        mode = Mode;
}
void EnvelopeFollower::reset()
{
    y_1 = 0;
}

