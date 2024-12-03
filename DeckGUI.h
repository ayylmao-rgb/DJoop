#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "GraphController.h"
#include "../Source/GraphController.h"

class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener, 
                   public FileDragAndDropTarget, 
                   public Timer,
    public GraphController::Listener
{
public:
    DeckGUI(int _deck_id,
        DJAudioPlayer* player, 
        AudioFormatManager & 	formatManagerToUse,
        AudioThumbnailCache & 	cacheToUse ,
        FilterController& filterController);

    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

     /** implement Button::Listener */
    void buttonClicked (Button *) override;

    /** implement Slider::Listener */
    void sliderValueChanged (Slider *slider) override;

    /**Implement CoordinatePlot::Listener */
    void cordinateChanged(GraphController* graphController) override;

    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override; 

    void timerCallback() override; 

    // purpose : load a track to the deck
    // input : url of the track
    // output : void
    void loadTrack(juce::URL audioURL);

private:

    // id of the dj deck
    int deck_id;

    // position slider
    Slider posSlider;

    // waveform display
    WaveformDisplay waveformDisplay;

    // load button
    TextButton loadButton{ "LOAD TRACK" };

    // player buttons
    TextButton playButton{">"};
    TextButton stopButton{"[]"};
    TextButton pauseButton{ "||" };
    TextButton forwardtButton{ ">>" };
    TextButton backwardButton{ "<<" };

    // rotary sliders
    Slider volSlider;
    Slider speedSlider;
    Slider LPFSlider;
    Slider HPFSlider;

    // slider lables
    Label volSliderLabel;
    Label speedSliderLabel;
    Label LPFSliderLabel;
    Label HPFSliderLabel;

    // Graphs
    GraphController graphController1;
    GraphController graphController2;

    // File chooser
    FileChooser fChooser{"Select Track"}; 

    // dj player of the deck
    DJAudioPlayer* player; 

    // filter controller component
    FilterController& filterController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
