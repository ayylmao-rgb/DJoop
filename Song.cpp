#include "Song.h"
#include <filesystem>

// Constructor definition for the Song class.
// It initializes the file, title, and URL members of the class with the given file parameter.
// The title is derived from the file name without extension, and the URL is constructed from the file object.
// It also logs the creation of a new track.
Song::Song(juce::File _file)
    : file(std::move(_file)), // Move the file object to avoid unnecessary copies
    title(file.getFileNameWithoutExtension()), // Initialize title with the filename without its extension
    URL(juce::URL{ file }) // Initialize URL with a juce::URL object created from the file
{

}

// Overload the equality operator to compare a Song object with a juce::String object (presumably a title).
// It returns true if the title member of the Song object is equal to the input juce::String object.
bool Song::operator==(const juce::String& other) const
{
    // Compare the title member with the input string and return the result
    return title == other;
}
