/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainAndDspAudioProcessor::GainAndDspAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                    valueTree(*this, nullptr, "Parameters", createParameters()),
                    lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 100, 0.1f))
#endif
{
}

GainAndDspAudioProcessor::~GainAndDspAudioProcessor()
{
}

//==============================================================================
const juce::String GainAndDspAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainAndDspAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainAndDspAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainAndDspAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainAndDspAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainAndDspAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainAndDspAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainAndDspAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GainAndDspAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainAndDspAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GainAndDspAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // 
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();

    dynamicWaveshaper.prepare(spec);

    updateParams();

    SingleEqBandProcessor::Band toneControlEqBand(
        "High",
        SingleEqBandProcessor::FilterType::Peak,
        2450.0f, // frequency
        1.1f,    // q
        juce::Decibels::decibelsToGain(0));
    toneControlEqProcessor.setBand(toneControlEqBand);
    toneControlEqProcessor.prepare(spec);

    lowPassFilter.reset();
    lowPassFilter.prepare(spec);

    irProcessor.reset();
    irProcessor.prepare(spec);
}

void GainAndDspAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void GainAndDspAudioProcessor::updateFilter()
{
    auto filterCutoffValue = valueTree.getRawParameterValue("FILTER_CUTOFF");
    float filterCutoff = filterCutoffValue->load();

    auto filterResValue = valueTree.getRawParameterValue("FILTER_RES");
    float filterRes = filterResValue->load();

    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, filterCutoff, filterRes);

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainAndDspAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GainAndDspAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //filter processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context = juce::dsp::ProcessContextReplacing<float>(block);

    updateFilter();
    lowPassFilter.process(context);

    updateParams();
    distortionProcessor.process(context);

    dynamicWaveshaper.process(context);

    toneControlEqProcessor.process(context);

    context.isBypassed = true;
    irProcessor.process(context);
}


void GainAndDspAudioProcessor::updateParams()
{
    //get the current slider values
    auto driveValue = valueTree.getRawParameterValue("GAIN_DRIVE");
    float drive = driveValue->load();

    auto rangeValue = valueTree.getRawParameterValue("GAIN_RANGE");
    float range = rangeValue->load();

    auto blendValue = valueTree.getRawParameterValue("GAIN_BLEND");
    float blend = blendValue->load();

    auto volumeValue = valueTree.getRawParameterValue("GAIN_VOLUME");
    float volume = volumeValue->load();

    distortionProcessor.setValues(drive, range, blend, volume);

    auto toneValue = valueTree.getRawParameterValue("TONE_VALUE");
    float tone = toneValue->load();

    // Line equation for f(0) = -10 and f(10) = 5.4 (empirically tested values that sound good)
    float toneBandGainInDb = 1.54 * tone - 10;
    toneControlEqProcessor.setBandGain(juce::Decibels::decibelsToGain(toneBandGainInDb));

    //cabinet
    auto cabBypassValue = valueTree.getRawParameterValue("CAB_ACTIVE");
    bool cabIsActive = cabBypassValue->load();
    irProcessor.setBypass(!cabIsActive);

    auto cabBoxSelectValue = valueTree.getRawParameterValue("CAB_BOX_SELECT");
    int cabBoxSelect = cabBoxSelectValue->load();

    if (cabBoxSelect != cabBoxSelectOldValue) 
    {
        irProcessor.setCabSelectId(cabBoxSelect);
        cabBoxSelectOldValue = cabBoxSelect;
    }

    //auto cabSelectValue = valueTree.getRawParameterValue("CAB_SELECT");
    //bool cabOneSelected = cabSelectValue->load();

    //if (cabOneSelected != cabSelectValueOld) 
    //{
    //    irProcessor.setSelection(cabOneSelected);
    //    cabSelectValueOld = cabOneSelected;
    //}
}

//==============================================================================
bool GainAndDspAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainAndDspAudioProcessor::createEditor()
{
    return new GainAndDspAudioProcessorEditor (*this);
}

//==============================================================================
void GainAndDspAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = valueTree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GainAndDspAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTree.state.getType()))
            valueTree.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainAndDspAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout GainAndDspAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF", "FilterCutoff", 20.0f, 500.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_RES", "FilterRes", 0.1f, 1.0f, 0.1f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_DRIVE", "GainDrive", 0.f, 1.f, 0.0001));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_RANGE", "GainRange", 0.f, 3000.f, 0.0001));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_BLEND", "GainBlend", 0.f, 1.f, 0.0001));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_VOLUME", "GainVolume", 0.f, 3.f, 0.0001));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TONE_VALUE", "ToneValue", 0.0f, 10.0f, 0.1f));

    params.push_back(std::make_unique<juce::AudioParameterBool>("CAB_ACTIVE", "CabActive", true));
    params.push_back(std::make_unique<juce::AudioParameterInt>("CAB_BOX_SELECT", "CabBoxSelect", 1, 3, 1));
    //params.push_back(std::make_unique<juce::AudioParameterBool>("CAB_SELECT", "CabSelect", true));

    return { params.begin(), params.end() };
}