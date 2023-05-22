#include <JuceHeader.h>
#include "WavetableSynth.h"



WavetableSynth::WavetableSynth() {

}

std::vector<float> WavetableSynth::generateSineWaveTable()
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<juce::InputStream> stream = std::make_unique<juce::MemoryInputStream>(BinaryData::sound1_wav, BinaryData::sound1_wavSize, false);
    juce::AudioFormatReader* reader = formatManager.createReaderFor(std::move(stream));

    juce::AudioBuffer<float> audioBuffer;
    long samepleLength = reader->lengthInSamples;
//    long samepleLength = pow(2, 100);
    audioBuffer.setSize(reader->numChannels, samepleLength);
    reader->read(&audioBuffer, 0, samepleLength, 0, true, true);

    std::vector<float> sineWaveTable = std::vector<float>(audioBuffer.getNumSamples());

    for(auto i = 0; i < samepleLength; i++) {
        sineWaveTable[i] = audioBuffer.getSample(0, i);
    }
    
//    constexpr auto WAVETABLE_LENGTH = 64;
//    const auto PI = std::atanf(1.f) * 4;
//    std::vector<float> sineWaveTable = std::vector<float>(WAVETABLE_LENGTH);
//
//    auto temp = 0.f;
//    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
//    {
//        temp = std::sinf(2 * PI * static_cast<float>(i) / WAVETABLE_LENGTH);
//        if(temp > 0) {
//            sineWaveTable[i] = 1.f;
//        }
//        else {
//            sineWaveTable[i] = -1.f;
//        }
//    }

    return sineWaveTable;
}

void WavetableSynth::initializeOscillators()
{
    oscillators.clear();
    constexpr auto OSCILLATOR_COUNT = 128;
    const auto sineWaveTable = generateSineWaveTable();

    for (auto i = 0; i < OSCILLATOR_COUNT; ++i)
    {
        oscillators.emplace_back(sineWaveTable, sampleRate);
    }
}

void WavetableSynth::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;

    initializeOscillators();
}

void WavetableSynth::processBlock(juce::AudioBuffer<float>& buffer, 
                                  juce::MidiBuffer& midiMessages, float gainValue)
{
    gain = gainValue;
    auto currentSample = 0;

    for (const auto midiMetadata : midiMessages)
    {
        const auto message = midiMetadata.getMessage();
        const auto messagePosition = static_cast<int>(message.getTimeStamp());

        render(buffer, currentSample, messagePosition);
        currentSample = messagePosition;
        handleMidiEvent(message);
    }

    render(buffer, currentSample, buffer.getNumSamples());
}
float WavetableSynth::midiNoteNumberToFrequency(const int midiNoteNumber)
{
    constexpr auto A4_FREQUENCY = 440.f;
    constexpr auto A4_NOTE_NUMBER = 69.f;
    constexpr auto NOTES_IN_AN_OCTAVE = 12.f;
    return A4_FREQUENCY * std::powf(2, (static_cast<float>(midiNoteNumber) - A4_NOTE_NUMBER) / NOTES_IN_AN_OCTAVE);
}

void WavetableSynth::handleMidiEvent(const juce::MidiMessage& midiMessage)
{
    if (midiMessage.isNoteOn())
    {
        const auto oscillatorId = midiMessage.getNoteNumber();
//        const auto frequency = midiNoteNumberToFrequency(oscillatorId);
        const auto frequency = juce::MidiMessage::getMidiNoteInHertz (oscillatorId);
        oscillators[oscillatorId].setFrequency(frequency);
    }
    else if (midiMessage.isNoteOff())
    {
        const auto oscillatorId = midiMessage.getNoteNumber();
        oscillators[oscillatorId].stop();
    }
    else if (midiMessage.isAllNotesOff())
    {
        for (auto& oscillator : oscillators)
        {
            oscillator.stop();
        }
    }
}

void WavetableSynth::render(juce::AudioBuffer<float>& buffer, int beginSample, int endSample)
{
    auto* firstChannel = buffer.getWritePointer(0);
    for (auto& oscillator : oscillators)
    {
        if (oscillator.isPlaying())
        {
            for (auto sample = beginSample; sample < endSample; ++sample)
            {
                firstChannel[sample] += oscillator.getSample() * gain;
            }
        }
    }

    for (int channel = 1; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        std::copy(firstChannel + beginSample, firstChannel + endSample, channelData + beginSample);
    }
}