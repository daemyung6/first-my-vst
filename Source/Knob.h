#pragma once
#include <JuceHeader.h>

class Knob
{
public:
    Knob(juce::Image _img, int _x, int _y, juce::AudioParameterFloat* _valuePoint, juce::String name);
    ~Knob();
    bool isPress(int x, int y);
    int x, y;
    bool pressing = false;
    juce::Image img;
    juce::AudioParameterFloat* valuePoint;
    void paint(juce::Graphics& g);
    juce::String name;
private:
    
};
