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
    //addAndMakeVisible(driveKnob = new juce::Slider("Drive"));
    //driveKnob->setSliderStyle(juce::Slider::Rotary);
    //driveKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    //addAndMakeVisible(rangeKnob = new juce::Slider("Range"));
    //rangeKnob->setSliderStyle(juce::Slider::Rotary);
    //rangeKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    //addAndMakeVisible(blendKnob = new juce::Slider("Blend"));
    //blendKnob->setSliderStyle(juce::Slider::Rotary);
    //blendKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    //addAndMakeVisible(volumeKnob = new juce::Slider("Volume"));
    //volumeKnob->setSliderStyle(juce::Slider::Rotary);
    //volumeKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    //driveAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "drive", *driveKnob);
    //rangeAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "range", *rangeKnob);
    //blendAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "blend", *blendKnob);
    //volumeAttachement = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "volume", *volumeKnob);


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
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    //distortion
    g.drawText("Drive", ((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Range", ((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Blend", ((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Volume", ((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);

    //filter
    juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
    g.drawText("IIR Low Pass Filter", titleArea, juce::Justification::centredTop);
    g.drawText("Cutoff", 46, 70, 50, 25, juce::Justification::centredLeft);
    g.drawText("Resonance", 107, 70, 70, 25, juce::Justification::centredLeft);

    juce::Rectangle <float> area(25, 25, 150, 150);

    g.setColour(juce::Colours::yellow);
    g.drawRoundedRectangle(area, 20.0f, 2.0f);


}

void GainAndDspAudioProcessorEditor::resized()
{
    //driveKnob->setBounds(((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    //rangeKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    //blendKnob->setBounds(((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    //volumeKnob->setBounds(((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);

    driveKnob.setBounds(((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    rangeKnob.setBounds(((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    blendKnob.setBounds(((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
    volumeKnob.setBounds(((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);


    //need to come back and dynamically set these...ok for now
    juce::Rectangle<int> area = getLocalBounds().reduced(40);

    filterCutoffDial.setBounds(30, 90, 70, 70);
    filterResDial.setBounds(100, 90, 70, 70);
}
