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
    
    setSize (400, 300);
}

WavetableSynthAudioProcessorEditor::~WavetableSynthAudioProcessorEditor()
{
    audioProcessor.onEditorClose();
}

//==============================================================================
void WavetableSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText (std::to_string(audioProcessor.gainValue), getLocalBounds(), juce::Justification::centred, 1);
    
    
    
    juce::String isPressStr;
    if(isMousePress) {
        isPressStr = "true";
    }
    else {
        isPressStr = "false";
    }
    
    juce::Rectangle<int> pos1 (0, 0, getWidth(), 100);
    g.drawFittedText (
        "isMousePress: "
        + isPressStr + " "
        + std::to_string(mousePos[0]) + " "
        + std::to_string(mousePos[1]) + ""
    , pos1, juce::Justification::topLeft, 1);
    
    juce::AffineTransform transform (juce::AffineTransform::translation
        (
            (float) knobImg.getWidth()  / -2.0f,
            (float) knobImg.getHeight() / -2.0f
        )
        .rotated ((audioProcessor.gainValue * 360.f) * 3.14f / 180.f)
        .translated (
            knobImgX, knobImgY
        )
    );
    
    
    g.drawImageTransformed(knobImg, transform, false);
}

void WavetableSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out  the positions of any
    // subcomponents in your editor..
}

//void WavetableSynthAudioProcessorEditor::sliderValueChanged (juce::Slider *slider) {
//    if ((slider == &gainSlider) && isPressGainSlider) {
//        auto value = static_cast<float>(gainSlider.getValue());
//        audioProcessor.setGainValue(&value, sizeof(value));
//        return;
//    }
//}
//
//void WavetableSynthAudioProcessorEditor::sliderDragStarted (juce::Slider *slider) {
//    if (slider == &gainSlider) {
//        isPressGainSlider = true;
//        return;
//    }
//}
//
//void WavetableSynthAudioProcessorEditor::sliderDragEnded (juce::Slider *slider) {
//    if (slider == &gainSlider) {
//        isPressGainSlider = false;
//        return;
//    }
//}
void WavetableSynthAudioProcessorEditor::mouseDrag(const juce::MouseEvent &event) {
    juce::Point<int> point = event.getPosition();
    auto x = point.getX();
    auto y = point.getY();
    
    if(!isMousePress) {
        mousePos[0] = x;
        mousePos[1] = y;
        
        if(
           (knobImgX - (knobImg.getWidth() / 2) < x) &&
           (x < knobImgX + (knobImg.getWidth() / 2)) &&
           (knobImgY - (knobImg.getHeight() / 2) < y) &&
           (y < knobImgY + (knobImg.getHeight() / 2))
           ) {
               knobImgIsPress = true;
           }
    }
    isMousePress = true;
    
    if(knobImgIsPress) {
        float v = audioProcessor.gainValue + static_cast<float>(mousePos[1] - y) / 100.f;
        if(v > 1.f) {
            v = 1.f;
        }
        else if(v < 0.f) {
            v = 0.f;
        }
        audioProcessor.setGainValue(&v, sizeof(v));
    }
    
    
    mousePos[0] = x;
    mousePos[1] = y;
}
void WavetableSynthAudioProcessorEditor::mouseUp(const juce::MouseEvent &event) {
    isMousePress = false;
    
    knobImgIsPress = false;
}
