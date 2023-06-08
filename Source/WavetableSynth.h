#pragma once
#include <JuceHeader.h>

#include "WavetableOscillator.h"

class WavetableSynth
{
public:
    WavetableSynth();
    void prepareToPlay(double sampleRate);
    void processBlock(
        juce::AudioBuffer<float>& buffer,
        juce::MidiBuffer& midiMessages,
        float gainValue,
        float att,
        float dec,
        float sus,
        float rel
    );

private:
    static std::vector<float> generateSineWaveTable();
    static float midiNoteNumberToFrequency(int midiNoteNumber);
    void initializeOscillators();
    void handleMidiEvent(const juce::MidiMessage& midiMessage);
    void render(juce::AudioBuffer<float>& buffer, int beginSample, int endSample);

    double sampleRate;
    float gain = 0.f;
    
    int attSec = 10;
    int decSec = 10;
    
    
    
    std::vector<WavetableOscillator> oscillators;
};
