#pragma once

#include <JuceHeader.h>
#include "EnvelopeFollower.h"

class DynamicWaveshaper
{
  public:
    virtual ~DynamicWaveshaper();
    virtual void prepare(const juce::dsp::ProcessSpec &spec) noexcept;
    void setAttackTime(float attackTime);
    void setReleaseTime(float releaseTime);
    void reset() noexcept;
    void process(const juce::dsp::ProcessContextReplacing<float> &context) noexcept;
    float processSample(int channel, int sample, float x, std::shared_ptr<juce::AudioBuffer<float>> sidechainBuffer);
    void processWithSidechain(const juce::dsp::ProcessContextReplacing<float>& context, std::shared_ptr<juce::AudioBuffer<float>> sidechainBuffer) noexcept;

  protected:
    float sampleRate;
    int maximumBlockSize;
    int numChannels;
    std::shared_ptr<juce::AudioBuffer<float>> envOutputBuffer;
    std::shared_ptr<juce::dsp::AudioBlock<float>> envOutputBlock;
    EnvelopeFollower envelopeFollower;
};
