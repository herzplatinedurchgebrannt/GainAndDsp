#include <JuceHeader.h>

using namespace juce::dsp;
#include "CabConvolutionProcessor.h"

void CabConvolutionProcessor::prepare(const ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    gain.setGainDecibels(6);
    convolution.prepare(spec);

    // BEGIN block
    // This block used to be a part of the updateParameters function, but since we're not changing
    // IRs dynamically I moved all of this into prepare.
    juce::AudioFormatManager manager;
    manager.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader{ manager.createReaderFor(
        std::make_unique<juce::MemoryInputStream>(BinaryData::cabsim_wav, BinaryData::cabsim_wavSize, true)
    ) };
    if (reader == nullptr)
    {
        jassertfalse;
        return;
    }
    juce::AudioBuffer<float> buffer(static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples));
    reader->read(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), 0, buffer.getNumSamples());
    bufferTransfer.set(BufferWithSampleRate{ std::move(buffer), reader->sampleRate });
    // END block

    // Load the IR. Note that this doesn't lock or allocate!
    bufferTransfer.get([this](BufferWithSampleRate& buf) {
        convolution.loadImpulseResponse(
            std::move(buf.buffer),
            buf.sampleRate,
            Convolution::Stereo::yes,
            Convolution::Trim::yes,
            Convolution::Normalise::yes);
        });
}

void CabConvolutionProcessor::process(ProcessContextReplacing<float> context)
{
    context.isBypassed = bypass;
    convolution.process(context);
    gain.process(context);
}

void CabConvolutionProcessor::reset()
{
    convolution.reset();
}
