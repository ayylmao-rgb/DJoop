#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include"FilterController.h"

class DJAudioPlayer : public AudioSource {
  public:

    // constructor
    DJAudioPlayer(AudioFormatManager& _formatManager);

    // deconstructor
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // purpose : load a url
    // input : url of the track
    // output : void
    void loadURL(URL audioURL);

    // purpose : set the volume
    // input : volume level
    // output : void
    void setGain(double gain);

    // purpose : set the speed of the track
    // input : url of the track
    // output : void
    void setSpeed(double ratio);

    // purpose : set the position of the song
    // input : url of the track
    // output : void
    void setPosition(double posInSecs);

    // purpose : set the relative position of the song
    // input : url of the track
    // output : void
    void setPositionRelative(double pos);

    // new

    // purpose : Sets the amount of reverb
    // input : size fo the room
    // output : void
    void setRoomSize(float size);

    // purpose : Sets the amount of reverb for damping
    // input : damping value
    // output : void
    void setDamping(float dampingAmt);

    // purpose : Sets the amount of wet level
    // input : wet level
    // output : void
    void setWetLevel(float wetLevel);

    // purpose : Sets the amount of dry level
    // input : dry level
    // output : void
    void setDryLevel(float dryLevel);

    // purpose : get the lengh of the song
    // input : none
    // output : lenth of a song in seconds
    double getLengthInSeconds();

    // purpose : start playing
    // input : none
    // output : void
    void start();

    // purpose : pause playing
    // input : none
    // output : void
    void stop();

    // purpose : get the position of the song
    // input : none
    // output : relative posion of the song
    double getPositionRelative();

    // for adding the filter
    FilterController& getSoundController();

private:

    // resource for managing files
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;

    // for changing the song
    AudioTransportSource transportSource;

    // resampling
    ResamplingAudioSource resampleSource{&transportSource, false, 2};

    // for adding the low pass and high pass filter functionality
    FilterController filterController;
    
    // for adding wetness,dryness,roomsize and damping functionality
    juce::ReverbAudioSource reverbSource{ &resampleSource, false };
    juce::Reverb::Parameters reverbParameters;

};




