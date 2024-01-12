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

    juce::Image background;

    int slRow = 7;
    int slCol = 4;

    //filter
    juce::Slider filterCutoffDial;
    juce::Slider filterResDial;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResAttachment;

    //distortion
    juce::Slider driveKnob;
    juce::Slider rangeKnob;
    juce::Slider blendKnob;
    juce::Slider volumeKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rangeAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> blendAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachement;

    //tone
    juce::Slider toneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneValueAttachement;

    //eq
    juce::Slider eqPresenceKnob;
    juce::Slider eqBassKnob;
    juce::Slider eqMidKnob;
    juce::Slider eqTrebleKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqPresenceAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqBassAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqTrebleAttachement;

    //cabinet
    juce::TextButton cabBypass;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> cabBypassAttachement;

    juce::ComboBox cabComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cabComboBoxAttachement;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainAndDspAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainAndDspAudioProcessorEditor)
};
