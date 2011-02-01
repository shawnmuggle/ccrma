//
//  Shader.fsh
//  GOLIATH
//
//  Created by Michael Rotondo on 1/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
