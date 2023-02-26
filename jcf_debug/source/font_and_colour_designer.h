
class FontAndColorContent;

/**
 * https://github.com/jcredland/juce-toys/
 *
 * An instance of FontAndColourDesign will allow you to
 * control one font and one colour while your application
 * is running.
 *
 * This is handy if you are indecisive about how something
 * should look whilst writing your UI.
 */

class FontAndColourDesigner
    :
    public juce::Component
{
public:
    FontAndColourDesigner (juce::Component& parentComponent,
                           juce::Colour& colourToUpdate,
                           juce::Font& fontToUpdate);

    void paint (juce::Graphics& g) override;
    void mouseUp (const juce::MouseEvent&) override;
private:
    juce::DocumentWindow window;
    std::unique_ptr<FontAndColorContent> content;
};

