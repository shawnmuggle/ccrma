// UIButton.h
#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

/**
* A UIButton is a widget with a simple
* label that responds to mouse events.
*/

#include "UIWidget.h"

class UIButton : public UIWidget
{
public:
    // Type of callbacks to handle button-press "fire" events.
    typedef void (*FireCallback)(UIButton* button);

    //
    // Constructor: Initialize a UI button with
    // a given font face and label text.
    //
    UIButton(STFont* font,
             const std::string& text,
             FireCallback callback);

    virtual void Display();
    virtual void HandleClick();
    
    std::string Text() { return mText; }

private:
    // Font to use for text
    STFont* mFont;

    // Text
    std::string mText;

    // "Fire" event callback
    FireCallback mCallback;
};

#endif // __UIBUTTON_H__
