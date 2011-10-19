//
//  UIDraggableBox.cpp
//  MicroUI
//
//  Created by Michael Rotondo on 10/19/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#include <iostream>

#include "UIDraggableBox.h"

UIDraggableBox::UIDraggableBox(STColor4f color) : UIBox(color)
{
    
}

void UIDraggableBox::HandleMouseDown(const STPoint2& position)
{
    UIWidget::HandleMouseDown(position);
    mRelativeMousePosition = position - GetRectangle().pMin;
}

void UIDraggableBox::HandleMouseMove(const STPoint2& position)
{
    MouseState state = CurrentMouseState();
    if (state == OVER || state == DOWN)
    {
        STVector2 diff = GetRectangle().pMax - GetRectangle().pMin;
        STPoint2 lowerLeft = position - mRelativeMousePosition;
        SetRectangle(UIRectangle(lowerLeft, lowerLeft + diff));
    }
}