#pragma once

#include <JuceHeader.h>

class Song
{
public:
    Song(juce::File _file);
    juce::File file;
    juce::URL URL;
    juce::String title;
    juce::String length;

    bool operator==(const juce::String& other) const;
};
