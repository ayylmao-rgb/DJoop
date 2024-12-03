#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

DeckGUI::DeckGUI(int _id,DJAudioPlayer* _player, 
                AudioFormatManager & 	formatManagerToUse,
                AudioThumbnailCache & 	cacheToUse,
                FilterController& filterController
           ) : 
                deck_id(_id),
                player(_player), 
                waveformDisplay(formatManagerToUse, cacheToUse),
                filterController(filterController)
{
    // load button
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    juce::Colour newTextColor = juce::Colour(0x00, 0x00, 0x00); // White color

    // waveform display
    addAndMakeVisible(waveformDisplay);

    // adding the sliding waveform functionality
    waveformDisplay.onPositionChanged = [this](double position) {
        player->setPositionRelative(position);
        posSlider.setValue(position);
    };

    // position slider config
    addAndMakeVisible(posSlider);
    posSlider.setRange(0.0, 1.0, 0.01);
    posSlider.addListener(this);
    
    // player buttons config
    addAndMakeVisible(backwardButton);
    backwardButton.addListener(this);

    addAndMakeVisible(pauseButton);
    pauseButton.addListener(this);

    addAndMakeVisible(playButton);
    playButton.addListener(this);
    
    addAndMakeVisible(stopButton);
    stopButton.addListener(this);
    
    addAndMakeVisible(forwardtButton);
    forwardtButton.addListener(this);

    // rotary sliders config
    addAndMakeVisible(volSlider);
    volSlider.addListener(this);
    volSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    volSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    volSlider.setRange(0.0, 1.0,0.01);
    volSlider.setValue(1);
    volSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke);
    volSlider.setDoubleClickReturnValue(true, 0.5);

    addAndMakeVisible(speedSlider);
    speedSlider.addListener(this);
    speedSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    speedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    speedSlider.setRange(0.0, 10.0, 0.1);
    speedSlider.setValue(1);
    speedSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke);
    speedSlider.setDoubleClickReturnValue(true, 0.0);

    addAndMakeVisible(LPFSlider);
    LPFSlider.addListener(this);
    LPFSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    LPFSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    LPFSlider.setRange(20.0,20000.0,0.1);
    LPFSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke);
    LPFSlider.setDoubleClickReturnValue(true, 0.0);

    addAndMakeVisible(HPFSlider);
    HPFSlider.addListener(this);
    HPFSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    HPFSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    HPFSlider.setRange(20.0, 20000.0, 0.1);
    HPFSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke);
    HPFSlider.setDoubleClickReturnValue(true, 0.0);

    // rotary slider labels config
    addAndMakeVisible(volSliderLabel);
    volSliderLabel.setText("Vol",dontSendNotification);
    volSliderLabel.attachToComponent(&volSlider, false);
    volSliderLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(speedSliderLabel);
    speedSliderLabel.setText("Speed", dontSendNotification);
    speedSliderLabel.attachToComponent(&speedSlider, false);
    speedSliderLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(LPFSliderLabel);
    LPFSliderLabel.setText("LPF", dontSendNotification);
    LPFSliderLabel.attachToComponent(&LPFSlider, false);
    LPFSliderLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(HPFSliderLabel);
    HPFSliderLabel.setText("HPF", dontSendNotification);
    HPFSliderLabel.attachToComponent(&HPFSlider, false);
    HPFSliderLabel.setJustificationType(Justification::centred);

    // add graph controllers config
    addAndMakeVisible(graphController1);
    graphController1.addListener(this);
    graphController1.setTooltip("Set reverbe");
    graphController1.setLabelText("", "RoomSize v Damping");

    addAndMakeVisible(graphController2);
    graphController2.addListener(this);
    graphController2.setTooltip("Set reverbe");
    graphController2.setLabelText("", "WetLevel v DryLevel");

    // setting the timer
    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
}

void DeckGUI::resized()
{
    // calculate parameters for placing the components
    double plotRowH = getWidth() / 2;
    double rowH = (getHeight() - plotRowH)/ 8;
    double colHButton = getWidth() / 5;
    double colHSliders = getWidth() / 4;
    
    // load button
    loadButton.setBounds(0, 0, getWidth(), rowH);

    // waveform
    waveformDisplay.setBounds(0, rowH, getWidth(), rowH * 1);

    // position slider
    posSlider.setBounds(0,rowH*2,getWidth(),rowH);
    
    // player buttons
    backwardButton.setBounds(0, rowH * 3, colHButton, rowH);
    pauseButton.setBounds(colHButton, rowH * 3, colHButton, rowH);
    playButton.setBounds(2*colHButton, rowH * 3, colHButton, rowH);
    stopButton.setBounds(3 * colHButton, rowH * 3, colHButton, rowH);
    forwardtButton.setBounds(4 * colHButton, rowH * 3, colHButton, rowH);

    // rotary sliders
    volSlider.setBounds(0, rowH *  5, colHSliders, rowH * 3);
    speedSlider.setBounds(colHSliders, rowH * 5, colHSliders, rowH * 3);
    LPFSlider.setBounds(2*colHSliders, rowH * 5, colHSliders, rowH*3);
    HPFSlider.setBounds(3*colHSliders, rowH * 5, colHSliders, rowH * 3);

    // graph controllers
    graphController1.setBounds(0, getHeight()-plotRowH, getWidth() / 2, plotRowH);
    graphController2.setBounds(getWidth() / 2, getHeight() - plotRowH,getWidth()/2, plotRowH);

}

void DeckGUI::buttonClicked(Button* button)
{
    // play button listener
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        player->start();
    }

    // stop button listener
    if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;

        // set the position of the player and stop it
        // adjust the position slider
        player->setPositionRelative(0);
        posSlider.setValue(0);
        player->stop();

    }
    if (button == &pauseButton)
    {
        // pause palying
        std::cout << "Pause button was clicked " << std::endl;
        player->stop();

    }
    if (button == &forwardtButton)
    {
        std::cout << "Next button was clicked " << std::endl;
        float value = player->getPositionRelative() + 0.02;

        // move the playing position slightly
        
        if (value <= 1) {
            
            player->setPositionRelative(value);
            posSlider.setValue(value);
        }
        else {
            player->setPositionRelative(1);
            posSlider.setValue(1);
        }
    }
    if (button == &backwardButton)
    {
        std::cout << "Prev button was clicked " << std::endl;
        float value = player->getPositionRelative() - 0.02;

        // move the playing position slightly to back

        if (value>=0) {
            player->setPositionRelative(value);
            posSlider.setValue(value);
        }
        else {
            player->setPositionRelative(0);
            posSlider.setValue(0);
        }
    }
    if (button == &loadButton)
    {
        auto fileChooserFlags = 
        FileBrowserComponent::canSelectFiles;

        // load the file to the deck from the filedirectory

        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
        {
            player->loadURL(URL{chooser.getResult()});
            // and now the waveformDisplay as well
            waveformDisplay.loadURL(URL{chooser.getResult()}); 
        });
    }
}

void DeckGUI::sliderValueChanged (Slider *slider)
{

    // activate the lowpass filter when the value is changed
    if (slider == &LPFSlider)
    {
        std::cout << "Slider LPF " << std::endl;
        player->getSoundController().setLowPassFrequency(slider->getValue());
    }

    // activate the highpass filter when value is changed
    if (slider == &HPFSlider)
    {
        std::cout << "Slider HPF " << std::endl;
        player->getSoundController().setHighPassFrequency(slider->getValue());
    }

    // change the speed slider value and adjust the speed
    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
        std::cout << "Speed Slider" << std::endl;
    }

    // change the volume slider and adjust the volume
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }

    // change the pos slider
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
    
}

void DeckGUI::cordinateChanged(GraphController* graphController)
{
    // handle the grpahs
    std::cout << "graph is being clicked" << std::endl;

    // when the user clicks first graph controller
    // change the room size and the damping
    if (graphController == &graphController1)
    {
        player->setDamping(graphController->getX());
        player->setRoomSize(graphController->getY());
    }

    // when the user clicks second graph controller
    // change the wetness and dryness
    if (graphController == &graphController2)
    {
        player->setDryLevel(graphController->getX());
        player->setWetLevel(graphController->getY());
    }
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
    // file dragging option
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
    // file dropped option
    std::cout << "DeckGUI::filesDropped" << std::endl;
    if (files.size() == 1)
    {
    player->loadURL(URL{File{files[0]}});
    }
}

void DeckGUI::timerCallback()
{
    // wavefrom postion changing
    waveformDisplay.setPositionRelative(
            player->getPositionRelative());
}

void DeckGUI::loadTrack(juce::URL audioURL)
{
    // load the file to the deck
    std::cout << "DeckGUI::loadFile called" << std::endl;
    player->loadURL(audioURL);
    waveformDisplay.loadURL(audioURL);
}


    

