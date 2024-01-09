/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GainAndDspAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GainAndDspAudioProcessorEditor (GainAndDspAudioProcessor&);
    ~GainAndDspAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    int slRow = 7;
    int slCol = 4;

    //distortion
    juce::Slider driveKnob;
    juce::Slider rangeKnob;
    juce::Slider blendKnob;
    juce::Slider volumeKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rangeAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> blendAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachement;

    //filter
    juce::Slider filterCutoffDial;
    juce::Slider filterResDial;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResAttachment;

    //tone
    juce::Slider toneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneValueAttachement;


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainAndDspAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainAndDspAudioProcessorEditor)
};
