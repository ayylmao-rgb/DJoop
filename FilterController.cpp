#include "FilterController.h"
#include "JuceHeader.h"

// set the lastSampleRate and currentSampleRates to default values
FilterController::FilterController()
    : lastSampleRate(44100.0), currentSampleRate(44100.0)
{
}

FilterController::~FilterController()
{
}

// purpose: setting up initial values
// inputs: sample rate of the track  number of data points of a block
// output : none
void FilterController::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<uint32_t> (samplesPerBlock), 2 };

    lowPassFilter.prepare(spec);
    lowPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 20000.0f);

    bandPassFilter.prepare(spec);
    bandPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 1000.0f, 0.7f);

    highPassFilter.prepare(spec);
    highPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);
}

// purpose: set the low pass frequency values
// input: frequency value for the low passing
// output : none
void FilterController::setLowPassFrequency(double frequency)
{
    if (frequency <= 0.0 || frequency > 20000.0)
    {
        std::cerr << " Wrong Frequency value. It should be in the range (0, 20000]" << std::endl;

        // Set a fallback value
        frequency = 1000.0;
    }

    auto values = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, frequency);
    lowPassFilter.coefficients = values;
}

// purpose: set the high pass frequency values
// input: frequency value for the high passing
// output : none
void FilterController::setHighPassFrequency(double frequency)
{
    if (frequency <= 0.0 || frequency > 20000.0)
    {
        std::cerr << " Wrong Frequency value. It should be in the range (0, 20000]" << std::endl;

        // Set a fallback value
        frequency = 500.0;
    }

    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, frequency);
    highPassFilter.coefficients = coefficients;

}

// purpose: apply filters to the audio bloack
// input: audio buffer for processing
// output : none
void FilterController::processAudioBlock(juce::AudioBuffer<float>& buffer)
{
    // check for channels and samples
    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0)
    {
        std::cerr << "Invalid channel and sample info" << std::endl;
        return;
    }

    // get channels individually
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        processSingleChannel(buffer.getWritePointer(channel), buffer.getNumSamples());
    }
}

// purpose: process a channel
// input: data array pointer and the number of samples in it
// output : none
void FilterController::processSingleChannel(float* channelData, int numSamples)
{
    // audio block for data points
    juce::dsp::AudioBlock<float> audioBlock(&channelData, 1, numSamples);

    // filtered audioblock creation
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    // create the filtered values
    lowPassFilter.process(context);
    highPassFilter.process(context);
}
