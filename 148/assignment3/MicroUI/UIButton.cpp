// UIButton.cpp
#include "UIButton.h"

#include "st.h"
#include "stgl.h"

//
// Constructor: Initialize a UI button with
// a given font face and label text.
//
UIButton::UIButton(STFont* font,
                   const std::string& text,
                   FireCallback callback)
    : mFont(font)
    , mText(text)
    , mCallback(callback)
{
}

void UIButton::Display()
{
    STColor4f c;
    switch (CurrentMouseState()) {
        case OVER:
            c = STColor4f(0.5f, 0.5f, 0.5f);
            break;
        case DOWN:
            c = STColor4f(0.3f, 0.3f, 0.3f);
            break;
        case IDLE:
        default:
            c = STColor4f(0.6f, 0.6f, 0.6f);
            break;
    }
    UIRectangle r = GetRectangle();
    
    glColor4f(c.r, c.g, c.b, c.a);
    glPushMatrix();
    glTranslatef(GetRectangle().pMin.x, GetRectangle().pMin.y, 0.0f);
    glRectf(0, 0, r.pMax.x - r.pMin.x, r.pMax.y - r.pMin.y);
    
    mFont->DrawString(mText, STColor4f(1.0f, 1.0f, 1.0f));
    glPopMatrix();
}

void UIButton::HandleClick()
{
    mCallback(this);
}