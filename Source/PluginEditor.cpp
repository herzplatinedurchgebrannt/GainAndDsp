/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainAndDspAudioProcessorEditor::GainAndDspAudioProcessorEditor (GainAndDspAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(700, 400);

    //distortion
    driveKnob.setSliderStyle(juce::Slider::Rotary);
    driveKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&driveKnob);
    driveAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_DRIVE", driveKnob);

    rangeKnob.setSliderStyle(juce::Slider::Rotary);
    rangeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&rangeKnob);
    rangeAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_RANGE", rangeKnob);

    blendKnob.setSliderStyle(juce::Slider::Rotary);
    blendKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&blendKnob);
    blendAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_BLEND", blendKnob);

    volumeKnob.setSliderStyle(juce::Slider::Rotary);
    volumeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    addAndMakeVisible(&volumeKnob);
    volumeAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_VOLUME", volumeKnob);

    //filter
    filterCutoffDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffDial.setRange(20.0f, 20000.0f);
    filterCutoffDial.setValue(600.0f);
    filterCutoffDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterCutoffDial.setPopupDisplayEnabled(true, true, this);
    filterCutoffDial.setSkewFactorFromMidPoint(1000.0f);
    addAndMakeVisible(&filterCutoffDial);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "FILTER_CUTOFF", filterCutoffDial);

    filterResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterResDial.setRange(0.1f, 1.0f);
    filterResDial.setValue(2.0f);
    filterResDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterResDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&filterResDial);
    filterResAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "FILTER_RES", filterResDial);


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
}

GainAndDspAudioProcessorEditor::~GainAndDspAudioProcessorEditor()
{
}

//==============================================================================
void GainAndDspAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::mintcream);
    
    g.setColour(juce::Colours::black);
    juce::Rectangle <float> areaTop(25, 25, 650, 150);
    g.drawRoundedRectangle(areaTop, 20.0f, 2.0f);

    juce::Rectangle <float> areaBottom(25, 200, 650, 150);
    g.drawRoundedRectangle(areaBottom, 20.0f, 2.0f);

    g.setColour(juce::Colours::black);
    g.setFont(15.0f);
    juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
    g.drawText("lx noise plugin", titleArea, juce::Justification::centredTop);

    //distortion
    g.drawText("Drive",  ((getWidth() / slRow) * 1) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Range",  ((getWidth() / slRow) * 2) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Blend",  ((getWidth() / slRow) * 3) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Volume", ((getWidth() / slRow) * 4) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);

    //filter
    g.drawText("Cutoff",    ((getWidth() / slRow) * 5) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Resonance", ((getWidth() / slRow) * 6) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
}

void GainAndDspAudioProcessorEditor::resized()

{
    //need to come back and dynamically set these...ok for now
    juce::Rectangle<int> areaTop = getLocalBounds().reduced(40);
    juce::Rectangle<int> areaBottom = getLocalBounds().reduced(40);

    //distortionm
    driveKnob.setBounds( ((getWidth() / slRow) * 1) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
    rangeKnob.setBounds( ((getWidth() / slRow) * 2) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
    blendKnob.setBounds( ((getWidth() / slRow) * 3) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
    volumeKnob.setBounds(((getWidth() / slRow) * 4) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);

    //filter
    filterCutoffDial.setBounds(((getWidth() / slRow) * 5) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
    filterResDial.setBounds(((getWidth() / slRow) * 6) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
}
