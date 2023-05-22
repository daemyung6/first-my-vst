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
class WavetableSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
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
    int mousePos[2] = {0, 0};
    int lastMousePos[2] = {0, 0};
    
    int knobImgX = 50.f;
    int knobImgY = 100.f;
    juce::Image knobImg;
    bool knobImgIsPress = false;
    
    void mouseDrag (const juce::MouseEvent &event);
    void mouseUp(const juce::MouseEvent &event);

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthAudioProcessorEditor)
};
