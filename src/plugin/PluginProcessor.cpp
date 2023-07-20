// Aria Stein July 2023
// Uses template code from Nathan Blair June 2023

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../parameters/StateManager.h"
#include "../audio/Gain.h"
#include <cmath>
#define MAX_WAVE_SIZE 2048
#define MAX_WAVE_AMOUNT 256

//==============================================================================
PluginProcessor::PluginProcessor()
{
    state = std::make_unique<StateManager>(this);
}

PluginProcessor::~PluginProcessor()
{
    // stop any threads, delete any raw pointers, remove any listeners, etc
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Called after the constructor, but before playback starts
    // Use this to allocate up any resources you need, and to reset any
    // variables that depend on sample rate or block size

    in_gain = std::make_unique<Gain>(float(sampleRate), samplesPerBlock, getTotalNumOutputChannels(), PARAMETER_DEFAULTS[PARAM::GAIN] / 100.0f);
    gain = std::make_unique<Gain>(float(sampleRate), samplesPerBlock, getTotalNumOutputChannels(), PARAMETER_DEFAULTS[PARAM::GAIN] / 100.0f);
    shaping_wave = std::make_unique<juce::AudioBuffer<double>>(1, MAX_WAVE_SIZE);
    shaping_wave->clear();
    //Test square wave for waveshaping, remove once loading is implemented
    for (int i = 0; i < MAX_WAVE_SIZE / 2; i++) {
        shaping_wave->setSample(0, i, .9);
        shaping_wave->setSample(0, (MAX_WAVE_SIZE - i) - 1, -.9);
    }
    wavetable = std::make_unique<juce::AudioBuffer<double>>(1, MAX_WAVE_SIZE * MAX_WAVE_AMOUNT);
    for (int i = 0; i < MAX_WAVE_AMOUNT; i++) {     // Initialize wavetable to ramps
        for (int j = 0; j < MAX_WAVE_SIZE; i++) {
        double new_sample = (((MAX_WAVE_SIZE - 1) / j) * 2.0) - 1.0;
        wavetable->setSample(1, (i * MAX_WAVE_SIZE) + j, new_sample);
        }
    }
    wavetable_index = std::make_unique<double>;
    *wavetable_index = 0.0;

}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    //--------------------------------------------------------------------------------
    // read in some parameter values here, if you want
    // in this case, gain goes from 0 to 100 (see: ../parameters/parameters.csv)
    // so we normalize it to 0 to 1
    //--------------------------------------------------------------------------------
    auto requested_gain = state->param_value(PARAM::GAIN) / 100.0f;
    auto requested_in_gain = state->param_value(PARAM::INGAIN) / 100.0f;

    //--------------------------------------------------------------------------------
    // process samples below. use the buffer argument that is passed in.
    // for an audio effect, buffer is filled with input samples, and you should fill it with output samples
    // for a synth, buffer is filled with zeros, and you should fill it with output samples
    // see: https://docs.juce.com/master/classAudioBuffer.html
    //--------------------------------------------------------------------------------

    in_gain->setGain(requested_in_gain);
    in_gain->process(buffer);

    //Do waveshaping stuff here
    for (int i = 0; i < buffer.getNumChannels(); i++) {
        for (int j = 0; j < buffer.getNumSamples(); j++) {
            double in_sample = buffer.getSample(i, j);
            double wave_index = in_sample + 1.0;
            wave_index /= 2.0;
            wave_index *= ((double)MAX_WAVE_SIZE - 1.0); //turn sample into wave index
            int higher_sample = ceil(wave_index);
            int lower_sample = floor(wave_index);
            int higher_wave = ceil(*wavetable_index);
            int lower_wave = floor(*wavetable_index);
            double higher_scale = fmod(wave_index, 1.0);
            double lower_scale = 1.0 - higher_scale;
            double higher_wave_scale = fmod(*wavetable_index, 1.0);
            double lower_wave_scale = 1.0 - higher_wave_scale;
            double higher_wave_sample = (wavetable->getSample(0, (higher_wave * MAX_WAVE_SIZE) + higher_sample)
                                         * higher_scale) + (wavetable->getSample(0, (higher_wave * MAX_WAVE_SIZE) + lower_sample) * lower_scale);
            double lower_wave_sample = (wavetable->getSample(0, (lower_wave * MAX_WAVE_SIZE) + higher_sample)
                                         * higher_scale) + (wavetable->getSample(0, (lower_wave * MAX_WAVE_SIZE) + lower_sample) * lower_scale);
            double out_sample = (higher_wave_sample * higher_wave_scale) + (lower_wave_sample * lower_wave_scale);
//            double out_sample = (shaping_wave->getSample(0, higher_sample) * higher_scale) + (shaping_wave->getSample(0, lower_sample) * lower_scale);
            buffer.setSample(i, j, out_sample);
        }
    }

    gain->setGain(requested_gain);
    gain->process(buffer);
    //--------------------------------------------------------------------------------
    // you can use midiMessages to read midi if you need.
    // since we are not using midi yet, we clear the buffer.
    //--------------------------------------------------------------------------------
    midiMessages.clear();
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // We will just store our parameter state, for now
    auto plugin_state = state->get_state();
    std::unique_ptr<juce::XmlElement> xml (plugin_state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // Restore our parameters from file
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    state->load_from(xmlState.get());
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
