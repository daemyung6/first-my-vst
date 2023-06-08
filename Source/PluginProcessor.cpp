/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), lowPassFilter( juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1f) )
#endif
{
    
    addParameter (freq = new juce::AudioParameterFloat ("freq", // parameterID
                                                         "freq", // parameter name
                                                         0.0f,   // minimum value
                                                         100.0f,   // maximum value
                                                         100.0f)
    );
    
    addParameter (res = new juce::AudioParameterFloat ("res", // parameterID
                                                         "res", // parameter name
                                                         0.0f,   // minimum value
                                                       100.0f,   // maximum value
                                                       0.0f)
    );
    addParameter (att = new juce::AudioParameterFloat ("att", // parameterID
                                                         "att", // parameter name
                                                         0.0f,   // minimum value
                                                       100.0f,   // maximum value
                                                       0.0f)
    );
    
    addParameter (dec = new juce::AudioParameterFloat ("dec", // parameterID
                                                         "dec", // parameter name
                                                         0.0f,   // minimum value
                                                       100.0f,   // maximum value
                                                       50.0f)
    );
    addParameter (sus = new juce::AudioParameterFloat ("sus", // parameterID
                                                         "sus", // parameter name
                                                         0.0f,   // minimum value
                                                       100.0f,   // maximum value
                                                       100.0f)
    );
    addParameter (rel = new juce::AudioParameterFloat ("rel", // parameterID
                                                         "rel", // parameter name
                                                         0.0f,   // minimum value
                                                       100.0f,   // maximum value
                                                       1.0f)
    );
}

WavetableSynthAudioProcessor::~WavetableSynthAudioProcessor()
{
    
}

//==============================================================================
const juce::String WavetableSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavetableSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WavetableSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavetableSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WavetableSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavetableSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WavetableSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void WavetableSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WavetableSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesBlock)
{
    lastSampleRate = sampleRate;
    synth.prepareToPlay(sampleRate);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
}

void WavetableSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavetableSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void WavetableSynthAudioProcessor::updateFilter() {
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, (*freq / 100.0f) * 5000.0f + 2.f, 10.f * (*res / 100.0f) + 0.5f);
}

void WavetableSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    /* update param*/
    buffer.applyGain (*freq);
    buffer.applyGain (*res);
    buffer.applyGain (*att);
    buffer.applyGain (*dec);
    buffer.applyGain (*sus);
    buffer.applyGain (*rel);
    

    juce::ScopedNoDenormals noDenormals;

    for (auto i = 0; i < buffer.getNumChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    
    synth.processBlock(
        buffer,
        midiMessages,
        0.8f,
        *att,
        *dec,
        *sus,
        *rel
    );
    
    juce::dsp::AudioBlock<float> block (buffer);
    updateFilter();
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool WavetableSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WavetableSynthAudioProcessor::createEditor()
{
    editor = new WavetableSynthAudioProcessorEditor (*this);
    return editor;
}


//==============================================================================
void WavetableSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::XmlElement xml = juce::XmlElement("root");
    
    juce::XmlElement* child;
    
    child = xml.createNewChildElement("freq");
    child->setAttribute("value", freq->get());
    
    child = xml.createNewChildElement("res");
    child->setAttribute("value", res->get());
    
    child = xml.createNewChildElement("att");
    child->setAttribute("value", att->get());
    
    child = xml.createNewChildElement("dec");
    child->setAttribute("value", dec->get());
    
    child = xml.createNewChildElement("sus");
    child->setAttribute("value", sus->get());
    
    child = xml.createNewChildElement("rel");
    child->setAttribute("value", rel->get());
    
    
    juce::MemoryOutputStream (destData, true).writeString(xml.toString());
}

void WavetableSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
//     *freq = juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readFloat();
    juce::String xmlStr = juce::MemoryInputStream (data, static_cast<size_t> (sizeInBytes), false).readString();

    juce::XmlDocument myDocument = juce::XmlDocument(xmlStr);
    
    if (auto mainElement = myDocument.getDocumentElement()) {
        auto* p = mainElement.get();
        
        auto tagName = p->getTagName();
        text += "parse on\n";
        if(tagName == "root") {
            auto i = -1;
            
            while(1) {
                i++;
                auto* child = p->getChildElement(i);
                if(child == nullptr) { return; }
                
                juce::String name = child->getTagName();
                auto val = child->getAttributeValue(0);
                float v = val.getFloatValue();
                text += "name: " + name + " getString: " + val + " --> float: " + std::to_string(v) + "\n";
                
                if(name == "freq") {
                    *freq = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
                    continue;
                }
                else if(name == "res") {
                    *res = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
                    continue;
                }
                else if(name == "att") {
                    *att = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
                    continue;
                }
                else if(name == "dec") {
                    *dec = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
                    continue;
                }
                else if(name == "sus") {
                    *sus = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
                    continue;
                }
                else if(name == "rel") {
                    *rel = juce::MemoryInputStream (&v, static_cast<size_t> (sizeof(v)), false).readFloat();
                    continue;
                }
            }
        }
        else {
            return;
        }
    }
    else {
        juce::String error = myDocument.getLastParseError();
        text += "parse err: " + error + "\n";
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthAudioProcessor();
}
