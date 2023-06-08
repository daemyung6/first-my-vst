#include <cmath>
#include "JuceHeader.h"
#include "WavetableOscillator.h"
#include <cmath>

WavetableOscillator::WavetableOscillator(std::vector<float> waveTable, double sampleRate)
: waveTable{ std::move(waveTable) },
    sampleRate{ sampleRate }
{
    env.setSampleRate(sampleRate);
}

float WavetableOscillator::getSample()
{
    jassert(isPlaying());
    index = std::fmod(index, static_cast<float>(waveTable.size()));
    const auto sample = interpolateLinearly();
    index += indexIncrement;
    
    return sample;
}

void WavetableOscillator::setFrequency(float frequency)
{
//    indexIncrement = frequency * static_cast<float>(waveTable.size())
//                                        / (static_cast<float>(sampleRate) * pow(2, 8));
    
    indexIncrement = (frequency * pow(2, 8)) / (static_cast<float>(sampleRate));
}

void WavetableOscillator::stop()
{
    index = 0.f;
    indexIncrement = 0.f;
}

bool WavetableOscillator::isPlaying() const
{
    return indexIncrement != 0.f;
}

float WavetableOscillator::interpolateLinearly() const
{
    const auto truncatedIndex = static_cast<typename  decltype(waveTable)::size_type>(index);
    const auto nextIndex = static_cast<typename  decltype(waveTable)::size_type>
                                                            (std::ceil(index)) % waveTable.size();
    const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
    return waveTable[nextIndex] * nextIndexWeight + 
                            (1.f - nextIndexWeight) * waveTable[truncatedIndex];
}

void WavetableOscillator::setVelocity(juce::uint8 _vel) {
    vel = static_cast<float>(_vel / 172.0f);
}
