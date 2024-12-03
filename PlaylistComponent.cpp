#include <JuceHeader.h>
#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1,
    DeckGUI* _deckGUI2,
    DJAudioPlayer* _playerForParsingMetaData
)
    : deckGUI1(_deckGUI1),
    deckGUI2(_deckGUI2),
    metaDataPlayer(_playerForParsingMetaData)
{
    // Setup child components and initial settings

    // Setup import button
    addAndMakeVisible(importButton);
    importButton.addListener(this);

    // Setup buttons for adding tracks to decks
    addAndMakeVisible(addToPlayer1Button);
    addToPlayer1Button.addListener(this);

    addAndMakeVisible(addToPlayer2Button);
    addToPlayer2Button.addListener(this);

    // Setup table list
    addAndMakeVisible(library);
    library.getHeader().addColumn("Song Title", 1, 1);
    library.getHeader().addColumn("Playing Time", 2, 1);
    library.getHeader().addColumn("Remove?", 3, 1);
    library.setModel(this);
    loadPreviousLibrary();
}

PlaylistComponent::~PlaylistComponent()
{
    // Destructor
    savePlaylist();
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    // Paint method with custom drawing
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
    g.setColour(juce::Colours::red);
    g.setFont(14.0f);
}

void PlaylistComponent::resized()
{
    float rowButton = getHeight() * 0.15;
    float colButton = getWidth() / 3;

    // Set position and size of buttons
    importButton.setBounds(0, getHeight() - rowButton, colButton, rowButton);
    addToPlayer1Button.setBounds(colButton, getHeight() - rowButton, colButton, rowButton);
    addToPlayer2Button.setBounds(2 * colButton, getHeight() - rowButton, colButton, rowButton);

    // Set position and size of library
    library.setBounds(0, 0, getWidth(), getHeight() - rowButton);

    // Set column sizes of library
    library.getHeader().setColumnWidth(1, 12.8 * getWidth() / 20);
    library.getHeader().setColumnWidth(2, 5 * getWidth() / 20);
    library.getHeader().setColumnWidth(3, 2 * getWidth() / 20);
}

int PlaylistComponent::getNumRows()
{
    // Return the number of tracks available
    return tracks.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    // Set row background colors
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange);
    }
    else
    {
        g.fillAll(juce::Colours::darkgrey);
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    // Display track titles and lengths
    if (rowNumber < getNumRows())
    {
        if (columnId == 1)
        {
            g.drawText(tracks[rowNumber].title, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
        if (columnId == 2)
        {
            g.drawText(tracks[rowNumber].length, 2, 0, width - 4, height, juce::Justification::centred, true);
        }
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    // Create delete buttons in each row
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "Remove" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

// Callback for when any button in the playlist component is clicked.
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    // Import button clicked
    if (button == &importButton)
    {
        addSongsToPlaylist();
    }
    // Add to player 1 button clicked
    else if (button == &addToPlayer1Button)
    {
        loadSongToPlayer(deckGUI1);
    }
    // Add to player 2 button clicked
    else if (button == &addToPlayer2Button)
    {
        loadSongToPlayer(deckGUI2);
    }
    // Other buttons (Remove button)
    else
    {
        int id = std::stoi(button->getComponentID().toStdString());
        deleteSongFromPlaylist(id);
        library.updateContent();
    }
}

// Load a selected track into a specified player.
void PlaylistComponent::loadSongToPlayer(DeckGUI* deckGUI)
{
    int selectedRow{ library.getSelectedRow() };
    if (selectedRow != -1)
    {
        DBG("Adding: " << tracks[selectedRow].title << " to Player");
        deckGUI->loadTrack(tracks[selectedRow].URL);
    }
}

// Add songs to the playlist from file chooser
void PlaylistComponent::addSongsToPlaylist()
{
    fChooser.launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectMultipleItems, [this](const FileChooser& chooser)
    {

            for (const juce::File& file : chooser.getResults())
            {
                juce::String fileNameWithoutExtension{ file.getFileNameWithoutExtension() };
                if (!checkSongExistance(fileNameWithoutExtension)) // if not already loaded
                {
                    Song newTrack{ file };
                    juce::URL audioURL{ file };
                    newTrack.length = getLength(audioURL);
                    tracks.push_back(newTrack);


                    DBG("loaded file: " << newTrack.title);
                }
                library.updateContent();
            }
    });
}

// Check if a song with a given filename exists in the playlist
bool PlaylistComponent::checkSongExistance(juce::String fileNameWithoutExtension)
{
    return (std::find(tracks.begin(), tracks.end(), fileNameWithoutExtension) != tracks.end());
}

// Delete a song from the playlist
void PlaylistComponent::deleteSongFromPlaylist(int id)
{
    tracks.erase(tracks.begin() + id);
}

// Get the length of a song from its URL
juce::String PlaylistComponent::getLength(juce::URL audioURL)
{
    metaDataPlayer->loadURL(audioURL);
    double seconds{ metaDataPlayer->getLengthInSeconds() };
    juce::String minutes{ secondsToMinutes(seconds) };
    return minutes;
}

// Convert seconds to minutes:seconds format
juce::String PlaylistComponent::secondsToMinutes(double seconds)
{
    int secondsRounded{ int(std::round(seconds)) };
    juce::String min{ std::to_string(secondsRounded / 60) };
    juce::String sec{ std::to_string(secondsRounded % 60) };

    if (sec.length() < 2)
    {
        sec = sec.paddedLeft('0', 2);
    }
    return juce::String{ min + ":" + sec };
}

// Find a song in the playlist based on a search string
int PlaylistComponent::findInPlaylist(juce::String searchText)
{
    auto it = find_if(tracks.begin(), tracks.end(),
        [&searchText](const Song& obj) {return obj.title.contains(searchText); });
    int i = -1;

    if (it != tracks.end())
    {
        i = std::distance(tracks.begin(), it);
    }

    return i;
}

// Save the playlist to a file
void PlaylistComponent::savePlaylist()
{
    std::ofstream myLibrary("my-library.csv");

    for (Song& t : tracks)
    {
        myLibrary << t.file.getFullPathName() << "," << t.length << "\n";
    }
}

// Load the playlist from a file
void PlaylistComponent::loadPreviousLibrary()
{
    std::ifstream myLibrary("my-library.csv");
    std::string filePath;
    std::string length;

    if (myLibrary.is_open())
    {
        while (getline(myLibrary, filePath, ',')) {
            juce::File file{ filePath };
            Song newTrack{ file };

            getline(myLibrary, length);
            newTrack.length = length;
            tracks.push_back(newTrack);
        }
    }
    myLibrary.close();
}
