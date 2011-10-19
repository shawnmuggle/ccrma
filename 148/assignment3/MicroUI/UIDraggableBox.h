//
//  UIDraggableBox.h
//  MicroUI
//
//  Created by Michael Rotondo on 10/19/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//

#ifndef MicroUI_UIDraggableBox_h
#define MicroUI_UIDraggableBox_h

#include "UIBox.h"

class UIDraggableBox : public UIBox {
public:
    UIDraggableBox(STColor4f color);
    virtual void HandleMouseDown(const STPoint2& position);
    virtual void HandleMouseMove(const STPoint2& position);
private:
    STVector2 mRelativeMousePosition;
};

#endif
