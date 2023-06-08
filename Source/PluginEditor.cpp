/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

//==============================================================================
WavetableSynthAudioProcessorEditor::WavetableSynthAudioProcessorEditor (WavetableSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    knobImg = juce::ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);
    
    
    
    knobs.emplace_back(Knob(knobImg, 0, 0, audioProcessor.freq, "freq"));
    knobs.emplace_back(Knob(knobImg, 100, 0, audioProcessor.res, "res"));
    knobs.emplace_back(Knob(knobImg, 0, 100, audioProcessor.att, "att"));
    knobs.emplace_back(Knob(knobImg, 100, 100, audioProcessor.dec, "dec"));
    knobs.emplace_back(Knob(knobImg, 0, 200, audioProcessor.sus, "sus"));
    knobs.emplace_back(Knob(knobImg, 100, 200, audioProcessor.rel, "rel"));
    
    setSize (400, 600);
    startTimer(1000 / 30);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor()
{
//    audioProcessor.onEditorClose();
}

//==============================================================================
void WavetableSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    for(auto i = 0; i < knobs.size(); i++) {
        knobs[i].paint(g);
    }
    
    juce::Rectangle<int> textPosition (0, 300, 9999, 9999);
    g.setColour (juce::Colours::white);

    g.drawFittedText (audioProcessor.text, textPosition, juce::Justification::left, 1);
}

void WavetableSynthAudioProcessorEditor::resized()
{

}

void WavetableSynthAudioProcessorEditor::mouseDrag(const juce::MouseEvent &event) {
    juce::Point<int> point = event.getPosition();
    auto x = point.getX();
    auto y = point.getY();
    
    if(!isMousePress) {
        mousePos[0] = x;
        mousePos[1] = y;
    }
    isMousePress = true;
    for (auto i = 0; i < knobs.size(); i++) {
        if(knobs[i].isPress(x, y) && !isKnobsPress) {
            knobs[i].pressing = true;
            isKnobsPress = true;
        }
    }
    
    for (auto i = 0; i < knobs.size(); i++) {
        if(knobs[i].pressing)
        {
            float v = static_cast<float>(*knobs[i].valuePoint);
            v = v + static_cast<float>(mousePos[1] - y) * 0.1f;
            if(v > 100.0f) {
                v = 100.0f;
            }
            else if(v < 0.0f) {
                v = 0.0f;
            }
            *knobs[i].valuePoint = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
        }
    }
    
    
    mousePos[0] = x;
    mousePos[1] = y;
}
void WavetableSynthAudioProcessorEditor::mouseUp(const juce::MouseEvent &event) {
    isMousePress = false;
    isKnobsPress = false;
    for(auto i = 0; i < knobs.size(); i++) {
        knobs[i].pressing = false;
    }
}
void WavetableSynthAudioProcessorEditor::timerCallback() {
    repaint();
}
