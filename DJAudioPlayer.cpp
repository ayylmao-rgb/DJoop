#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) 
: formatManager(_formatManager)
{
    // set the initial paramters to default values
    reverbParameters.roomSize = 0;
    reverbParameters.damping = 0;
    reverbParameters.wetLevel = 0;
    reverbParameters.dryLevel = 1.0;
    reverbSource.setParameters(reverbParameters);
}

DJAudioPlayer::~DJAudioPlayer()
{

}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    reverbSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void DJAudioPlayer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    reverbSource.getNextAudioBlock(bufferToFill);
    filterController.processAudioBlock(*bufferToFill.buffer);

}
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
    reverbSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    // this is a good file
    if (reader != nullptr)
    {       
        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, 
true)); 
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);             
        readerSource.reset (newSource.release());          
    }
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1" << std::endl;
    }
    else {
        transportSource.setGain(gain);
    }
   
}
void DJAudioPlayer::setSpeed(double ratio)
{
  if (ratio < 0 || ratio > 100.0)
    {
        std::cout << "DJAudioPlayer::setSpeed ratio should be between 0 and 100" << std::endl;
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
     if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1" << std::endl;
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
}
void DJAudioPlayer::stop()
{
  transportSource.stop();
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

FilterController& DJAudioPlayer::getSoundController()
{
    return filterController;
}


// set the room size
void DJAudioPlayer::setRoomSize(float size)
{
    std::cout << "DJAudioPlayer::setRoomSize triggered" << std::endl;
    if (size < 0 || size > 1.0)
    {
        std::cout << "DJAudioPlayer::setRoomSize size should be between 0 and 1.0" << std::endl;
    }
    else
    {
        reverbParameters.roomSize = size;
        reverbSource.setParameters(reverbParameters);
    }
}

// set the damping value
void DJAudioPlayer::setDamping(float dampingAmt)
{
    std::cout << "DJAudioPlayer::setDamping called" << std::endl;
    if (dampingAmt < 0 || dampingAmt > 1.0)
    {
        std::cout << "DJAudioPlayer::setDamping amount should be between 0 and 1.0" << std::endl;
    }
    else
    {
        reverbParameters.damping = dampingAmt;
        reverbSource.setParameters(reverbParameters);
    }
}

void DJAudioPlayer::setWetLevel(float wetLevel)
{
    std::cout << "DJAudioPlayer::setWetLevel called" << std::endl;
    if (wetLevel < 0 || wetLevel > 1.0)
    {
        std::cout << "DJAudioPlayer::setWetLevel level should be between 0 and 1.0" << std::endl;
    }
    else
    {
        reverbParameters.wetLevel = wetLevel;
        reverbSource.setParameters(reverbParameters);
    }
}

void DJAudioPlayer::setDryLevel(float dryLevel)
{
    std::cout << "DJAudioPlayer::setDryLevel called" << std::endl;
    if (dryLevel < 0 || dryLevel > 1.0)
    {
        std::cout << "DJAudioPlayer::setDryLevel level should be between 0 and 1.0" << std::endl;
    }
    else
    {
        reverbParameters.dryLevel = dryLevel;
        reverbSource.setParameters(reverbParameters);
    }
}

double DJAudioPlayer::getLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}