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
class TheGrillerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TheGrillerAudioProcessorEditor (TheGrillerAudioProcessor&);
    ~TheGrillerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::Image background;

    int slRow = 7;
    int slCol = 4;

    //filter
    juce::Slider filterCutoffDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;

    //distortion
    juce::Slider driveKnob;
    juce::Slider rangeKnob;
    juce::Slider volumeKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rangeAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachement;

    //eq
    juce::Slider eqBassKnob;
    juce::Slider eqMidKnob;
    juce::Slider eqTrebleKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqBassAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqTrebleAttachement;

    //cabinet
    juce::TextButton cabBypass;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> cabBypassAttachement;
    juce::ComboBox cabComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cabComboBoxAttachement;

    //tone
    juce::Slider toneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneValueAttachement;

    //output 
    juce::Slider outputSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputValueAttachement;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TheGrillerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheGrillerAudioProcessorEditor)
};
