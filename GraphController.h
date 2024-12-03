#pragma once

#include <JuceHeader.h>

// visualizing a 2d graph 
class GraphController : public juce::Component,
    public juce::SettableTooltipClient
{
public:

    GraphController();

    ~GraphController() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    // purpose: displaying a number of grid lines
    // inputs: number of grid lines to display in the plot
    // output : none
    void setGridLineCount(int lineCount = 4);

    // purpose: setting the range for axis
    // inputs: minimum and the maximum values of the axis
    // output : none
    void setRange(float min = 0.0f, float max = 1.0f);

    // purpose: getting the x value where the current mouse pointer at
    // inputs: none
    // output : x co-ordinate value
    float getX();

    // purpose: getting the y value where the current mouse pointer at
    // inputs: none
    // output : y co-ordinate value
    float getY();

    // this class is inside the controller class and listen for events
    class Listener
    {
    public:
        Listener();

        ~Listener();

        // call back for value changing
        virtual void cordinateChanged(GraphController* coordinatePlot) = 0;
    };

    // purpose: adding the listener
    // inputs: none
    // output : none
    void controlGraphComponent();

    // purpose: adding the listener to a listener compoennt
    // inputs: listner 
    // output : none
    void addListener(Listener* l);

    // purpose: removing the listener to a listener compoennt
    // inputs: listner 
    // output : none
    void removeListener(Listener* l);

    // purpose: set axis labels
    // inputs: x and y parameters
    // output : none
    void setLabelText(const juce::String& topText, const juce::String& bottomText);

private:
    // A list of listeners
    juce::ListenerList<Listener> listeners;

    // stores corredinates
    std::map<char, float> coordsRaw;

    // stores range
    std::map<char, float> rangeRaw;

    // purpose: set the range valus
    // inputs: none 
    // output : none
    void setRangeRaw();

    // purpose: create the corrdinates
    // inputs: rowx and y values
    // output : none
    void initCoords(float rawX, float rawY);

    // purpose: set the cordiante values
    // inputs: rowx and rowy values 
    // output : none
    void setCoords(float rawX, float rawY);

    // purpose: updating the corrdinates
    // inputs: listner 
    // output : none
    void updateCoords();

    // Positional settings variables
    float midY, midX, left, right, top, bottom;

    // purpose: setting the postings
    // inputs: listner 
    // output : none
    void setSettings();

    // User settings
    int gridLineCount; // The number of grid lines
    std::map<char, float> range; // A map to hold the range values

    // purpose : drawing the plots, axis , grids, marker, text
    // input : graphics
    // output : none
    void drawPlot(juce::Graphics& g);
    void drawAxis(juce::Graphics& g);
    void drawGrid(juce::Graphics& g);
    void drawMarker(juce::Graphics& g);
    void drawText(juce::Graphics& g);

    // checking on the mouse pointer
    bool markerMoved{ false };

    // purpose: setting the conditions
    // inputs: coordinates
    // output : none
    float constrain(float coord);

    // purpose: inverting the corrdinates 
    // inputs: corrdinate and the min and max values
    // output : none
    float invertCoord(float coord, float min, float max);

    // purpose: check if the cordinate in the range
    // inputs: coordinates
    // output : none
    bool inRangeRaw(float rawX, float rawY);

    // for printing axis
    juce::Label topLabel;
    juce::Label bottomLabel;

    // Macro to declare non-copyable class and leak detector
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphController)
};