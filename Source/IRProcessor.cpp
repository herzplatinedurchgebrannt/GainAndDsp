#include <JuceHeader.h>

using namespace juce::dsp;
#include "IRProcessor.h"

void IRProcessor::prepare(const ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    gain.setGainDecibels(6);
    convolution.prepare(spec);

    updateParameters();
}

void IRProcessor::process(ProcessContextReplacing<float> context)
{
    context.isBypassed = bypass;
    convolution.process(context);
    gain.process(context);
}

void IRProcessor::reset()
{
    convolution.reset();
}

void IRProcessor::setBypass(bool bypassValue) {
    bypass = bypassValue;
}

//void IRProcessor::setSelection(bool cabSelectionValue) {
//    cabOneIsSelected = cabSelectionValue;
//
//    updateParameters();
//}

void IRProcessor::setCabSelectId(int cabSelectNewValue) {
    cabSelectId = cabSelectNewValue;

    updateParameters();
}

void IRProcessor::updateParameters()
{
    //juce::AudioFormatManager manager;
    //manager.registerBasicFormats();


    //this approach is suited for external samples
    //juce::String assetName = (cabOneIsSelected == true ? "cabsim.wav" : "cabsim_2.wav");

    //juce::File file = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile);

    //auto audio1 = file.getChildFile("./../../../../../../Source/Assets/" + assetName);

    //auto x = audio1.createInputStream();
    // 
    //std::unique_ptr<juce::AudioFormatReader> reader{ manager.createReaderFor(
    //    std::move(x)
    //) };

    const char* currentCab;
    int currentCabSize;

    switch (cabSelectId)
    {
    case 1:
        currentCab = BinaryData::cabsim_wav;
        currentCabSize = BinaryData::cabsim_wavSize;
        break;
    case 2:
        currentCab = BinaryData::cabsim_2_wav;
        currentCabSize = BinaryData::cabsim_2_wavSize;
        break;    
    case 3:
        currentCab = BinaryData::cabsim_3_wav;
        currentCabSize = BinaryData::cabsim_3_wavSize;
        break;
    default:
        break;
    }

    juce::AudioFormatManager manager;
    manager.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader{ manager.createReaderFor(
        std::make_unique<juce::MemoryInputStream>(currentCab, currentCabSize, true)
    ) };


    if (reader == nullptr)
    {
        jassertfalse;
        return;
    }

    juce::AudioBuffer<float> buffer(static_cast<int> (reader->numChannels),
        static_cast<int> (reader->lengthInSamples));
    reader->read(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), 0, buffer.getNumSamples());

    bufferTransfer.set(BufferWithSampleRate{ std::move(buffer), reader->sampleRate });

    // Load a new IR if there's one available. Note that this doesn't lock or allocate!
    bufferTransfer.get([this](BufferWithSampleRate& buf)
        {
            convolution.loadImpulseResponse(std::move(buf.buffer),
            buf.sampleRate,
            Convolution::Stereo::yes,
            Convolution::Trim::yes,
            Convolution::Normalise::yes);
        });

}