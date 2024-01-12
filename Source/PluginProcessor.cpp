/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheGrillerAudioProcessor::TheGrillerAudioProcessor()
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
                    highPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 100, 0.1f))
#endif
{
}

TheGrillerAudioProcessor::~TheGrillerAudioProcessor()
{
}

//==============================================================================
const juce::String TheGrillerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheGrillerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TheGrillerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TheGrillerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TheGrillerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TheGrillerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TheGrillerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheGrillerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TheGrillerAudioProcessor::getProgramName (int index)
{
    return {};
}

void TheGrillerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TheGrillerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // 
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();

    highPassFilter.reset();
    highPassFilter.prepare(spec);

    dynamicWaveshaper.prepare(spec);

    updateParams();

    eqProcessor.prepare(spec);

    SingleEqBandProcessor::Band toneControlEqBand(
        "High",
        SingleEqBandProcessor::FilterType::Peak,
        2450.0f, // frequency
        1.1f,    // q
        juce::Decibels::decibelsToGain(0));
    toneControlEqProcessor.setBand(toneControlEqBand);
    toneControlEqProcessor.prepare(spec);

    eqProcessor.prepare(spec);

    irProcessor.reset();
    irProcessor.prepare(spec);

    outputLevelProcessor.reset();
    outputLevelProcessor.prepare(spec);
}

void TheGrillerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}



#ifndef JucePlugin_PreferredChannelConfigurations
bool TheGrillerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void TheGrillerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context = juce::dsp::ProcessContextReplacing<float>(block);

    updateParams();
    highPassFilter.process(context);
    distortionProcessor.process(context);
    dynamicWaveshaper.process(context);
    eqProcessor.process(context);

    //context.isBypassed = true;
    irProcessor.process(context);

    toneControlEqProcessor.process(context);

    outputLevelProcessor.process(context);
}


void TheGrillerAudioProcessor::updateParams()
{
    //highpass filter
    auto filterCutoffValue = valueTree.getRawParameterValue("FILTER_CUTOFF");
    float filterCutoff = filterCutoffValue->load();

    //auto filterResValue = valueTree.getRawParameterValue("FILTER_RES");
    //float filterRes = filterResValue->load();

    float filterRes = 0.35;

    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, filterCutoff, filterRes);

    //distortion
    auto driveValue = valueTree.getRawParameterValue("GAIN_DRIVE");
    float drive = driveValue->load();

    auto rangeValue = valueTree.getRawParameterValue("GAIN_RANGE");
    float range = rangeValue->load();

    //auto blendValue = valueTree.getRawParameterValue("GAIN_BLEND");
    //float blend = blendValue->load();
    float blend = 1.0f;

    auto volumeValue = valueTree.getRawParameterValue("GAIN_VOLUME");
    float volume = volumeValue->load();

    distortionProcessor.setValues(drive, range, blend, volume);

    //eq
    auto eqBassValue = valueTree.getRawParameterValue("EQ_BASS");
    float eqBass = eqBassValue->load();

    auto eqMidValue = valueTree.getRawParameterValue("EQ_MID");
    float eqMid = eqMidValue->load();

    auto eqHighValue = valueTree.getRawParameterValue("EQ_HIGH");
    float eqHigh = eqHighValue->load();

    EqProcessor::Band postEqLowest(
        "Lowest",
        EqProcessor::FilterType::HighPass,
        144.0f, // frequency
        1.1f    // q
    );
    EqProcessor::Band postEqLow(
        "Low",
        EqProcessor::FilterType::NoFilter,
        0.0f, // frequency
        0.0f, // q
        0.0f, // gain
        false // active
    );
    EqProcessor::Band postEqLowMids(
        "Low Mids",
        EqProcessor::FilterType::Peak,
        304.0f,                        // frequency
        1.1f,                          // q
        juce::Decibels::decibelsToGain(eqBass) // gain
    );
    EqProcessor::Band postEqHighMids(
        "High Mids",
        EqProcessor::FilterType::Peak,
        896.0f,                        // frequency
        1.1f,                          // q
        juce::Decibels::decibelsToGain(eqMid) // gain
    );
    EqProcessor::Band postEqHigh(
        "High",
        EqProcessor::FilterType::Peak,
        2680.0f,                        // frequency
        1.1f,                           // q
        juce::Decibels::decibelsToGain(eqHigh) // gain
    );
    EqProcessor::Band postEqHighest(
        "Highest",
        EqProcessor::FilterType::NoFilter,
        0.0f, // frequency
        0.0f, // q
        0.0f, // gain
        false // active
    );

    eqProcessor.setBand(0, postEqLowest);
    eqProcessor.setBand(1, postEqLow);
    eqProcessor.setBand(2, postEqLowMids);
    eqProcessor.setBand(3, postEqHighMids);
    eqProcessor.setBand(4, postEqHigh);
    eqProcessor.setBand(5, postEqHighest);
    eqProcessor.setGain(6);

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

    //tone
    auto toneValue = valueTree.getRawParameterValue("TONE_VALUE");
    float tone = toneValue->load();

    // Line equation for f(0) = -10 and f(10) = 5.4 (empirically tested values that sound good)
    float toneBandGainInDb = 1.54 * tone - 10;
    toneControlEqProcessor.setBandGain(juce::Decibels::decibelsToGain(toneBandGainInDb));

    auto outputValue = valueTree.getRawParameterValue("OUTPUT_VALUE");
    float output = outputValue->load();
    outputLevelProcessor.setGainLinear(juce::Decibels::decibelsToGain(output));
}

//==============================================================================
bool TheGrillerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheGrillerAudioProcessor::createEditor()
{
    return new TheGrillerAudioProcessorEditor (*this);
}

//==============================================================================
void TheGrillerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = valueTree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TheGrillerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new TheGrillerAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout TheGrillerAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF", "FilterCutoff", 20.0f, 500.0f, 5.0f));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_RES", "FilterRes", 0.1f, 1.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_DRIVE", "GainDrive", 0.f, 1.f, 0.0001));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_RANGE", "GainRange", 0.f, 3000.f, 0.0001));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_BLEND", "GainBlend", 0.f, 1.f, 0.0001));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN_VOLUME", "GainVolume", 0.f, 3.f, 0.0001));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EQ_BASS", "EqBass", -8.f, 8.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EQ_MID", "EqMid", -8.f, 8.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EQ_HIGH", "EqHigh", -8.f, 8.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("CAB_ACTIVE", "CabActive", true));
    params.push_back(std::make_unique<juce::AudioParameterInt>("CAB_BOX_SELECT", "CabBoxSelect", 1, 3, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TONE_VALUE", "ToneValue", 0.0f, 10.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTPUT_VALUE", "OutputValue", -30.0f, 10.0f, 0.1f));

    return { params.begin(), params.end() };
}