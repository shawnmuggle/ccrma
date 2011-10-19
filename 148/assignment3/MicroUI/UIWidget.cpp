// UIWidget.cpp
#include "UIWidget.h"

#include "st.h"
#include "stgl.h"

//
// Constructor: Initialize a UI widget.
//
UIWidget::UIWidget() : mOver(false), mDown(false)
{
}

//
// Destructor: Finalize a UI widget.
//
UIWidget::~UIWidget()
{
}

//
// Display the widget to the current OpenGL context.
//
void UIWidget::Display()
{
}

//
// Determine if this widget or any sub-widget
// is under the specified point.
//
bool UIWidget::HitTest(const STPoint2& position)
{
    return GetRectangle().Contains(position);
}

void UIWidget::HandleMouseDown(const STPoint2& position)
{
    printf("DOWN\n");
    mDown = true;
}

void UIWidget::HandleMouseUp(const STPoint2& position)
{
    if (mOver && mDown)
    {
        HandleClick();
    }
    printf("UP\n");
    mDown = false;
}

void UIWidget::HandleMouseEnter()
{
    printf("ENTER\n");
    mOver = true;
}

void UIWidget::HandleMouseLeave()
{
    printf("LEAVE\n");
    mOver = false;
}

void UIWidget::HandleClick()
{
    
}

void UIWidget::HandleMouseMove(const STPoint2& position) {}

UIWidget::MouseState UIWidget::CurrentMouseState()
{
    if (mDown && mOver)
        return DOWN;
    else if (mDown || mOver)
        return OVER;
    return IDLE;
}
