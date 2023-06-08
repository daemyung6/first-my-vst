/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Knob.h"


//==============================================================================
/**
*/
class WavetableSynthAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    WavetableSynthAudioProcessorEditor (WavetableSynthAudioProcessor&);
    ~WavetableSynthAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavetableSynthAudioProcessor& audioProcessor;
    
    bool isMousePress = false;
    bool isKnobsPress = false;
    int mousePos[2] = {0, 0};
    int lastMousePos[2] = {0, 0};
    
    int knobImgX = 50.f;
    int knobImgY = 100.f;
    juce::Image knobImg;
    bool knobImgIsPress = false;
    
    void mouseDrag (const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    
    void timerCallback() override;
    
    int temp_id = 0;
    float temp_value = 0.f;
    
    int nowPressKnobID = 0;
    
    std::vector<Knob> knobs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthAudioProcessorEditor)
};
