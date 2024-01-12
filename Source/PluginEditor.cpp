/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheGrillerAudioProcessorEditor::TheGrillerAudioProcessorEditor (TheGrillerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(700, 400);

    //filter
    filterCutoffDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffDial.setRange(20.0f, 500.0f);
    filterCutoffDial.setValue(100);
    filterCutoffDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    filterCutoffDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&filterCutoffDial);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "FILTER_CUTOFF", filterCutoffDial);

    //distortion
    driveKnob.setSliderStyle(juce::Slider::Rotary);
    driveKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    driveKnob.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&driveKnob);
    driveAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_DRIVE", driveKnob);

    rangeKnob.setSliderStyle(juce::Slider::Rotary);
    rangeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    rangeKnob.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&rangeKnob);
    rangeAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_RANGE", rangeKnob);

    volumeKnob.setSliderStyle(juce::Slider::Rotary);
    volumeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    volumeKnob.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&volumeKnob);
    volumeAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "GAIN_VOLUME", volumeKnob);

    //eq
    eqBassKnob.setSliderStyle(juce::Slider::Rotary);
    eqBassKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    eqBassKnob.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&eqBassKnob);
    eqBassAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "EQ_BASS", eqBassKnob);

    eqMidKnob.setSliderStyle(juce::Slider::Rotary);
    eqMidKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    eqMidKnob.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&eqMidKnob);
    eqMidAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "EQ_MID", eqMidKnob);

    eqTrebleKnob.setSliderStyle(juce::Slider::Rotary);
    eqTrebleKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    eqTrebleKnob.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&eqTrebleKnob);
    eqTrebleAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "EQ_HIGH", eqTrebleKnob);

    //cabinet
    cabBypass.setButtonText("Bypass");
    cabBypass.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkgrey);
    cabBypass.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::mintcream);
    cabBypass.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
    cabBypass.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
    cabBypass.setClickingTogglesState(true);
    addAndMakeVisible(cabBypass);
    cabBypassAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTree, "CAB_ACTIVE", cabBypass);

    cabComboBox.addItem("Cab 1", 1);
    cabComboBox.addItem("Cab 2", 2);
    cabComboBox.addItem("Cab 3", 3);
    addAndMakeVisible(cabComboBox);
    cabComboBoxAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTree, "CAB_BOX_SELECT", cabComboBox);

    //tone
    toneSlider.setSliderStyle(juce::Slider::Rotary);
    toneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    toneSlider.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&toneSlider);
    toneValueAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "TONE_VALUE", toneSlider);

    //output
    outputSlider.setSliderStyle(juce::Slider::Rotary);
    outputSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    outputSlider.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&outputSlider);
    outputValueAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTree, "OUTPUT_VALUE", outputSlider);
}

TheGrillerAudioProcessorEditor::~TheGrillerAudioProcessorEditor()
{
}

//==============================================================================
void TheGrillerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    //background = juce::ImageCache::getFromMemory(BinaryData::rust_png, BinaryData::rust_pngSize);
    //g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);

    g.fillAll(juce::Colours::mintcream);
    
    g.setColour(juce::Colours::black);
    juce::Rectangle <float> areaAmp(25, 25, 450, 150);
    g.drawRoundedRectangle(areaAmp, 20.0f, 2.0f);

    juce::Rectangle <float> areaCab(500, 25, 175, 150);
    g.drawRoundedRectangle(areaCab, 20.0f, 2.0f);

    juce::Rectangle <float> areaTone(25, 200, 650, 150);
    g.drawRoundedRectangle(areaTone, 20.0f, 2.0f);

    g.setColour(juce::Colours::black);
    g.setFont(15.0f);
    juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
    g.drawText("GUITAR AMP", titleArea, juce::Justification::centredTop);

    //filter
    g.drawText("Cutoff",    ((getWidth() / slRow) * 4) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);

    //distortion
    g.drawText("Drive",     ((getWidth() / slRow) * 1) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Range",     ((getWidth() / slRow) * 2) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);
    g.drawText("Volume",    ((getWidth() / slRow) * 3) - (100 / 2), (getHeight() / slCol) + 5, 100, 100, juce::Justification::centred, false);

    //eq
    g.drawText("Bass",      ((getWidth() / slRow) * 1) - (100 / 2), (getHeight() / slCol) + 180, 100, 100, juce::Justification::centred, false);
    g.drawText("Mid",       ((getWidth() / slRow) * 2) - (100 / 2), (getHeight() / slCol) + 180, 100, 100, juce::Justification::centred, false);
    g.drawText("Treble",    ((getWidth() / slRow) * 3) - (100 / 2), (getHeight() / slCol) + 180, 100, 100, juce::Justification::centred, false);

    //tone
    g.drawText("Tone",      ((getWidth() / slRow) * 5) - (100 / 2), (getHeight() / slCol) + 180, 100, 100, juce::Justification::centred, false);
    
    g.drawText("Output",    ((getWidth() / slRow) * 6) - (100 / 2), (getHeight() / slCol) + 180, 100, 100, juce::Justification::centred, false);
}

void TheGrillerAudioProcessorEditor::resized()

{
    juce::Rectangle<int> areaTop = getLocalBounds().reduced(40);
    juce::Rectangle<int> areaBottom = getLocalBounds().reduced(40);

    //filter
    filterCutoffDial.setBounds(((getWidth() / slRow) * 4) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);

    //distortion
    driveKnob.setBounds( ((getWidth() / slRow) * 1) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
    rangeKnob.setBounds( ((getWidth() / slRow) * 2) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);
    volumeKnob.setBounds(((getWidth() / slRow) * 3) - (100 / 2), (getHeight() / slCol) - (100 / 2), 100, 100);

    //eq
    eqBassKnob.setBounds(((getWidth() / slRow) * 1) - (100 / 2), (getHeight() / slCol) + 125, 100, 100);
    eqMidKnob.setBounds(((getWidth() / slRow) * 2) - (100 / 2), (getHeight() / slCol) + 125, 100, 100);
    eqTrebleKnob.setBounds(((getWidth() / slRow) * 3) - (100 / 2), (getHeight() / slCol) + 125, 100, 100);

    //cabinet
    cabBypass.setBounds(((getWidth() / slRow) * 6) - (100 / 2) - 10, (getHeight() / slCol) - (100 / 2), 100, 30);
    cabComboBox.setBounds(((getWidth() / slRow) * 6) - (100 / 2) - 10, (getHeight() / slCol) - (100 / 2) + 50, 100, 30);

    //tone 
    toneSlider.setBounds(((getWidth() / slRow) * 5) - (100 / 2), (getHeight() / slCol) + 125, 100, 100);

    //output 
    outputSlider.setBounds(((getWidth() / slRow) * 6) - (100 / 2), (getHeight() / slCol) + 125, 100, 100);
}
