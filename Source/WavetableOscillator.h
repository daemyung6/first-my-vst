#pragma once
#include <vector>
#include <JuceHeader.h>

class WavetableOscillator
{
public:
    WavetableOscillator(std::vector<float> waveTable, double sampleRate);
    WavetableOscillator(const WavetableOscillator&) = delete;
    WavetableOscillator& operator=(const WavetableOscillator&) = delete;
    WavetableOscillator(WavetableOscillator&&) = default;
    WavetableOscillator& operator=(WavetableOscillator&&) = default;

    float getSample();
    void setFrequency(float frequency);
    void stop();
    bool isPlaying() const;
    
    bool isDec = false;
    double dec = 0.f;
    bool isAtt = true;
    double att = 0.f;
    
    void setVelocity(juce::uint8 vel);
    
    juce::ADSR env;
    
    float vel = 0.0f;
private:
    float interpolateLinearly() const;

    float index = 0.f;
    float indexIncrement = 0.f;
    std::vector<float> waveTable;
    double sampleRate;
    
};
