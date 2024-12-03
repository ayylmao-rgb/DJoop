#include <JuceHeader.h>
#include "GraphController.h"
#include <iomanip>
#include <sstream>

GraphController::GraphController()
{
    // Initializes the coordinate plot for display and interaction
    // - Grid lines: count, ranges, and coordinates (default values)
    // - Labels: top and bottom (visible, justification)

    setGridLineCount();   // Sets number of grid lines (default)
    setRange();           // Sets initial plot range (default)
    initCoords(75.0f, 75.0f); // Initializes coordinate system (default origin)

    addAndMakeVisible(topLabel);
    addAndMakeVisible(bottomLabel);

    topLabel.setJustificationType(juce::Justification::centredTop);
    bottomLabel.setJustificationType(juce::Justification::centredBottom);
}

// deconstrucotor
GraphController::~GraphController() {}

GraphController::Listener::Listener() {}
GraphController::Listener::~Listener() {}

void GraphController::paint(juce::Graphics& g)
{
    // Visually renders the coordinate plot onto the provided graphics context
    // - Background: filled with window background color
    // - Plot: drawn in grey
    // - Marker: drawn in orange (and associated text if moved)
    // - Raw range: captured for resizing reference

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::grey);
    drawPlot(g);

    g.setColour(juce::Colours::orange);
    drawMarker(g);

    if (markerMoved) {
        g.setColour(juce::Colours::white);
        drawText(g);
    }

    setRangeRaw();
}


void GraphController::resized()
{
    // Handles component resizing by adjusting settings and coordinates
    // - Updates internal settings (likely for plot dimensions and markers)
    // - Resizes top and bottom labels to match new component bounds

    setSettings(); // Adjusts plot dimensions and markers
    updateCoords(); // Updates coordinates based on new size

    topLabel.setBounds(getLocalBounds()); // Resizes top label
    bottomLabel.setBounds(getLocalBounds()); // Resizes bottom label
}


// Handles mouse press events on the component
// - Logs clicked coordinates with a suggestion for accuracy
// - Updates marker position and interactive elements
// - Repaints for visual updates
void GraphController::mouseDown(const juce::MouseEvent& event)
{
    // Indicate marker movement
    markerMoved = true;

    // Hide mouse cursor while pressed
    setMouseCursor(juce::MouseCursor::NoCursor);

    // Position marker at clicked point
    setCoords(float(event.getMouseDownX()), float(event.getMouseDownY()));

    // Trigger interactions with listening components
    controlGraphComponent();

    // Repaint to display new marker position
    repaint();
}


// Handles mouse drag events, updating marker position and triggering interactions
// - Logs current drag coordinates
// - Retrieves mouse position
// - Updates marker coordinates based on drag position
// - Repaints for visual updates
void GraphController::mouseDrag(const juce::MouseEvent& event)
{
    // Log current drag coordinates
    DBG("Mouse dragged to: " << getX() << ", " << getY());

    // Get mouse position coordinates
    juce::Point<int> rawPos(event.getPosition());
    float rawX = float(rawPos.getX());
    float rawY = float(rawPos.getY());

    // Update marker position to follow mouse drag
    setCoords(rawX, rawY);

    // Trigger interactions with listening components
    controlGraphComponent();

    // Repaint to visualize marker changes
    repaint();
}


// -- MouseUp Event Handler --
// Restores mouse cursor to normal state on button release
void GraphController::mouseUp(const juce::MouseEvent& event)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

// -- Notification for Listeners --
// Triggers coordinateChanged events on all listeners for plot value changes
void GraphController::controlGraphComponent()
{
    listeners.call([this](Listener& l) { l.cordinateChanged(this); });
}

// -- Listener Management --
// Adds a listener to receive coordinateChanged events from this plot
void GraphController::addListener(Listener* l) { listeners.add(l); }

// Removes a listener, stopping its coordinationChanged event reception
void GraphController::removeListener(Listener* l) { listeners.remove(l); }

// -- Marker Coordinate Retrieval --
// Retrieves the constrained X coordinate of the marker
float GraphController::getX() { return constrain(coordsRaw['x']); }

// -- Marker Y Coordinate Retrieval --
// Retrieves the inverted and constrained Y coordinate of the marker (likely for visual representation)
float GraphController::getY()
{
    return invertCoord(constrain(coordsRaw['y']), range['min'], range['max']);
}

// -- Grid Line Count Configuration --
// Sets the grid line count, ensuring it's even (for visual symmetry)
void GraphController::setGridLineCount(int lineCount)
{
    // Enforce even grid lines (adjust if odd)
    gridLineCount = lineCount % 2 == 1 ? lineCount - 1 : lineCount;
}

// -- Plot Range Management --
// Sets the minimum and maximum values representable on the plot
void GraphController::setRange(float min, float max)
{
    range['min'] = min;
    range['max'] = max;
}

// -- Initial Marker Coordinates --
// Initializes the marker's X and Y coordinates
void GraphController::initCoords(float rawX, float rawY)
{
    coordsRaw['x'] = rawX;
    coordsRaw['y'] = rawY;
}

// -- Marker Coordinate Updates (with Range Check) --
// Sets the marker's coordinates if they fall within the valid range
void GraphController::setCoords(float rawX, float rawY)
{
    if (inRangeRaw(rawX, rawY)) {
        coordsRaw['x'] = rawX;
        coordsRaw['y'] = rawY;
    }
}

// -- Coordinate Updates on Resize --
// Adjusts marker coordinates based on initial range and component size changes
void GraphController::updateCoords()
{
    // Calculate ratios based on initial range
    double xRatio = double(coordsRaw['x'] / (rangeRaw['max'] - rangeRaw['min']));
    double yRatio = double(coordsRaw['y'] / (rangeRaw['max'] - rangeRaw['min']));

    // Calculate new coordinates based on current size and ratios
    float newX = float(right * xRatio);
    float newY = float(bottom * yRatio);

    setCoords(newX, newY);
}

// -- Raw Range Capture from Component Bounds --
// Sets the raw range (likely for internal calculations) based on component size
void GraphController::setRangeRaw()
{
    rangeRaw['min'] = getLocalBounds().getX();
    rangeRaw['max'] = getLocalBounds().getWidth();
}

// -- Plot Drawing --
// Renders the plot outline, axis, and grid lines
void GraphController::drawPlot(juce::Graphics& g)
{
    g.drawRect(getLocalBounds(), 3); // Outline the component
    drawAxis(g);
    drawGrid(g);
}

// -- Axis Drawing --
// Draws the X and Y axes on the plot
void GraphController::drawAxis(juce::Graphics& g)
{
    g.drawLine(left, midY, right, midY, 2); // X-axis
    g.drawLine(midX, left, midX, bottom, 2); // Y-axis
}

// Draws a grid on the plot.
// Parameters:
//   - g: a reference to a juce::Graphics object used for drawing
void GraphController::drawGrid(juce::Graphics& g)
{
    const float myDashLength[] = { 3, 3 };
    float offset = float(getLocalBounds().getWidth() / (gridLineCount + 2));

    for (int i = 0; i < (gridLineCount / 2); ++i)
    {
        int d{ i + 1 }; // Distance from the axis
        // Draw lines to the left/right of the Y-axis and top/bottom of the X-axis
        g.drawDashedLine(juce::Line<float>(midX - offset * d, top, midX - offset * d, bottom),
            &myDashLength[0], 2, 1.0, 0);
        g.drawDashedLine(juce::Line<float>(midX + offset * d, top, midX + offset * d, bottom),
            &myDashLength[0], 2, 1.0, 0);
        g.drawDashedLine(juce::Line<float>(left, midY - offset * d, right, midY - offset * d),
            &myDashLength[0], 2, 1.0, 0);
        g.drawDashedLine(juce::Line<float>(left, midY + offset * d, right, midY + offset * d),
            &myDashLength[0], 2, 1.0, 0);
    }
}

// Draws a marker on the plot.
// Parameters:
//   - g: a reference to a juce::Graphics object used for drawing
void GraphController::drawMarker(juce::Graphics& g)
{
    // Set the length of the cursor
    float length = float(getLocalBounds().getWidth() / 15);

    // Create horizontal and vertical lines
    juce::Line<float> lineH(juce::Point<float>(coordsRaw['x'] - length, coordsRaw['y']),
        juce::Point<float>(coordsRaw['x'] + length, coordsRaw['y']));
    juce::Line<float> lineV(juce::Point<float>(coordsRaw['x'], coordsRaw['y'] - length),
        juce::Point<float>(coordsRaw['x'], coordsRaw['y'] + length));

    // Draw the lines
    g.drawLine(lineH, 2.0f);
    g.drawLine(lineV, 2.0f);
}

// Draws the x and y coordinates as text on the plot.
// Parameters:
//   - g: a reference to a juce::Graphics object used for drawing
void GraphController::drawText(juce::Graphics& g)
{
    g.setFont(float(getWidth() / 12));
    int textHeight = int(g.getCurrentFont().getHeight());

    // Draw Y coordinate
    std::stringstream streamY;
    streamY << std::fixed << std::setprecision(2) << getY();
    g.drawText(streamY.str(), int(midX), int(top), int(midX), textHeight,
        juce::Justification::centredLeft, true);

    // Draw X coordinate
    std::stringstream streamX;
    streamX << std::fixed << std::setprecision(2) << getX();
    g.drawText(streamX.str(), int(midX), int(midY), int(midX), textHeight,
        juce::Justification::centredRight, true);
}

// Recalculates all settings, updating variables representing different points and boundaries within the local bounds of the component.
void GraphController::setSettings()
{
    // Recalculate all settings
    midY = float(getLocalBounds().getCentreY());
    midX = float(getLocalBounds().getCentreX());
    left = float(getLocalBounds().getX());
    right = float(getLocalBounds().getRight());
    top = float(getLocalBounds().getY());
    bottom = float(getLocalBounds().getBottom());
}

// Constrains a coordinate to be within a specific range, based on a transformation of ranges.
// Parameters:
//   - coord: the coordinate to be constrained
// Returns:
//   - the constrained coordinate
float GraphController::constrain(float coord)
{
    float oldRangeMin = float(getLocalBounds().getX());
    float oldRangeMax = float(getLocalBounds().getWidth());
    float oldRange = oldRangeMax - oldRangeMin;
    float newRange = range['max'] - range['min'];

    float newValue = (((coord - oldRangeMin) * newRange) / oldRange) + range['min'];
    return newValue;
}

// Inverts a coordinate within a specified range.
// Parameters:
//   - coord: the coordinate to be inverted
//   - min: the minimum value of the range
//   - max: the maximum value of the range
// Returns:
//   - the inverted coordinate
/** Inverts coord within a range between min and max */
float GraphController::invertCoord(float coord, float min, float max)
{
    return (min + max) - coord;
}

// Checks if the raw coordinates are within the valid range of the plot.
// Parameters:
//   - rawX: the raw X coordinate to be checked
//   - rawY: the raw Y coordinate to be checked
// Returns:
//   - true if the coordinates are within the range, false otherwise
bool GraphController::inRangeRaw(float rawX, float rawY)
{
    return (rawX >= left && rawX <= right && rawY >= top && rawY <= bottom);
}

void GraphController::setLabelText(const juce::String& topText, const juce::String& bottomText)
{
    // Sets the text of the top and bottom labels in the coordinate plot.
    // Parameters:
    //   const juce::String& topText: the text to be set for the top label
    //   const juce::String& bottomText: the text to be set for the bottom label
    // Output: Modifies the topLabel and bottomLabel components to display the specified text.

    topLabel.setText(topText, juce::dontSendNotification); // Automatically generated code
    bottomLabel.setText(bottomText, juce::dontSendNotification); // Automatically generated code

    // Disabling interception of mouse clicks on both labels
    topLabel.setInterceptsMouseClicks(false, false); // Automatically generated code
    bottomLabel.setInterceptsMouseClicks(false, false); // Automatically generated code
}



