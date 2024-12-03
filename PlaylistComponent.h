#pragma once

#include <JuceHeader.h>

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "Song.h"
#include "DeckGUI.h"
#include "DJAudioPlayer.h"


class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel,
    public juce::Button::Listener,
    public juce::TextEditor::Listener
{
public:
    PlaylistComponent(DeckGUI* _deckGUI1,
        DeckGUI* _deckGUI2,
        DJAudioPlayer* _playerForParsingMetaData
    );
    ~PlaylistComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected
    ) override;
    void paintCell(juce::Graphics& g,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected
    ) override;

    Component* refreshComponentForCell(int rowNumber,
        int columnId,
        bool isRowSelected,
        Component* existingComponentToUpdate) override;
    void buttonClicked(juce::Button* button) override;
private:
    // Table list box
    std::vector<Song> tracks;
    juce::TableListBox library;
       
    // buttons
    juce::TextButton importButton{ "IMPORT TRACKS" };
    juce::TextButton addToPlayer1Button{ "LOAD TO DECK 1" };
    juce::TextButton addToPlayer2Button{ "LOAD TO DECK 2" };
       
    // deckgui
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;

    // dj player
    DJAudioPlayer* metaDataPlayer;

    juce::String getLength(juce::URL audioURL);
    juce::String secondsToMinutes(double seconds);

    void addSongsToPlaylist();
    void savePlaylist();
    void loadPreviousLibrary();
    void deleteSongFromPlaylist(int id);
    bool checkSongExistance(juce::String fileNameWithoutExtension);
    int findInPlaylist(juce::String searchText);
    void loadSongToPlayer(DeckGUI* deckGUI);

    FileChooser fChooser{ "Select Track" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};



