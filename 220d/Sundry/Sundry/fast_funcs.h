//
//  sin_lookup.h
//  Sundry
//
//  Created by Michael Rotondo on 12/12/11.
//  Copyright (c) 2011 Rototyping. All rights reserved.
//


float sin_lookup(float phase);
float boosted_sin_lookup(float phase);

// These are only really worthwhile when x is around 0  :(
inline float fastexp3(float x) { 
    return (6+x*(6+x*(3+x)))*0.16666666f; 
}

inline float fastexp4(float x) {
    return (24+x*(24+x*(12+x*(4+x))))*0.041666666f;
}

inline float fastexp5(float x) {
    return (120+x*(120+x*(60+x*(20+x*(5+x)))))*0.0083333333f;
}

inline float fastexp6(float x) {
    return (720+x*(720+x*(360+x*(120+x*(30+x*(6+x))))))*0.0013888888f;
}

inline float fastexp7(float x) {
    return (5040+x*(5040+x*(2520+x*(840+x*(210+x*(42+x*(7+x)))))))*0.00019841269f;
}

inline float fastexp8(float x) {
    return (40320+x*(40320+x*(20160+x*(6720+x*(1680+x*(336+x*(56+x*(8+x))))))))*2.4801587301e-5;
}

inline float fastexp9(float x) {
    return (362880+x*(362880+x*(181440+x*(60480+x*(15120+x*(3024+x*(504+x*(72+x*(9+x)))))))))*2.75573192e-6;
}