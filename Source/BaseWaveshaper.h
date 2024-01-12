#pragma once
#include <JuceHeader.h>

class BaseWaveshaper
{
  public:
    virtual ~BaseWaveshaper();
    virtual void prepare(const juce::dsp::ProcessSpec &spec) noexcept;
    void reset() noexcept;
    virtual float processSample(float sample);
    void process(const juce::dsp::ProcessContextReplacing<float> &context) noexcept;

  protected:
    float sampleRate;
};
