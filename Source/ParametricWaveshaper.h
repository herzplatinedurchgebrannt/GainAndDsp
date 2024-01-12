#pragma once

#include <JuceHeader.h>
#include "BaseWaveshaper.h"

class ParametricWaveshaper
{
  public:
    ParametricWaveshaper();
    void prepare(const juce::dsp::ProcessSpec &spec);
    float processSample(int channel, int sample, float x, std::shared_ptr<juce::AudioBuffer<float>> sidechainBuffer);
    void processWithSidechain(
        const juce::dsp::ProcessContextReplacing<float> &context,
        std::shared_ptr<juce::AudioBuffer<float>> sidechainBuffer) noexcept;
};
