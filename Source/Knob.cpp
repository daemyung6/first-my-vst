#include "Knob.h"

Knob::Knob(juce::Image _img, int _x, int _y, juce::AudioParameterFloat* _valuePoint, juce::String _name) {
    x = _x;
    y = _y;
    valuePoint = _valuePoint;
    img = _img;
    name = _name;
}
Knob::~Knob() {
    
}
bool Knob::isPress(int inputx, int inputy) {
    if(
       (x < inputx) &&
       (inputx < x + img.getWidth()) &&
       (y < inputy) &&
       (inputy < y + img.getHeight())
    ) {
        return true;
    }
    return false;
}
void Knob::paint(juce::Graphics& g) {
    juce::AffineTransform transform (juce::AffineTransform::translation
        (
            (float) img.getWidth() / -2.0f,
            (float) img.getHeight() / -2.0f
        )
        .rotated ((((*valuePoint / 100.f) * 0.8f + 0.1f) * 360.f) * 3.14f / 180.f)
        .translated (
             x + img.getWidth() / 2, y + img.getHeight() / 2
        )
    );
    g.drawImageTransformed(img, transform, false);
    
    juce::Rectangle<int> textPosition (x, y, img.getWidth(), img.getHeight());
    g.setColour (juce::Colours::black);
    
    g.drawFittedText (name + " " + std::to_string(*valuePoint), textPosition, juce::Justification::centred, 1);
}
