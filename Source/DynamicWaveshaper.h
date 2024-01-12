#pragma once

#include <JuceHeader.h>
#include "EnvelopeFollower.h"
#include "ParametricWaveshaper.h"

class DynamicWaveshaper
{
  public:
    virtual ~DynamicWaveshaper();
    virtual void prepare(const juce::dsp::ProcessSpec &spec) noexcept;
    void setAttackTime(float attackTime);
    void setReleaseTime(float releaseTime);
    void reset() noexcept;
    void process(const juce::dsp::ProcessContextReplacing<float> &context) noexcept;

  protected:
    float sampleRate;
    int maximumBlockSize;
    int numChannels;
    std::shared_ptr<juce::AudioBuffer<float>> envOutputBuffer;
    std::shared_ptr<juce::dsp::AudioBlock<float>> envOutputBlock;
    EnvelopeFollower envelopeFollower;
    ParametricWaveshaper parametricWaveshaper;
};
