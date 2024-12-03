
#pragma once
#include <JuceHeader.h>
// FilterController class is responsible for adding the lowPass and highPass filtering mechanisms
class FilterController
{
public:

	FilterController();

	~FilterController();

	// purpose: setting up initial values
	// inputs: sample rate of the track number of data points of a block
	// output : none
	void prepareToPlay(double sampleRate, int samplesPerBlock);

	// purpose: set the low pass frequency values
	// input: frequency value for the low passing
	// output : none
	void setLowPassFrequency(double frequency);

	// purpose: set the high pass frequency values
	// input: frequency value for the high passing
	// output : none
	void setHighPassFrequency(double frequency);

	// purpose: apply filters to the audio bloack
	// input: audio buffer for processing
	// output : none
	void processAudioBlock(juce::AudioBuffer<float>& buffer);

	// purpose: process a channel
	// input: data array pointer and the number of samples in it
	// output : none
	void processSingleChannel(float* channelData, int numSamples);

private:
	// Filters and state variables for the audio processor (Self-written)
	juce::dsp::IIR::Filter<float> lowPassFilter;
	juce::dsp::IIR::Filter<float> bandPassFilter;
	juce::dsp::IIR::Filter<float> highPassFilter;

	// setting initial values
	double lowPassFrequency = 2000.0;
	double bandPassFrequency = 1000.0;
	double highPassFrequency = 500.0;

	double currentSampleRate;
	double lastSampleRate;
	int lastSamplesPerBlock;
};
