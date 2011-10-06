//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : AutoWah.cpp
// Created by   : music424 staff
// Company      : CCRMA - Stanford
// Description  : 
//
// Date         : 5/16/11
//-------------------------------------------------------------------------------------------------------

#include "AutoWah.h"
#include <math.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new AutoWah (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
AutoWah::AutoWah (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, kNumProgs, kNumParams)	// 1 program, 1 parameter only
{
	setNumInputs (kNumInputs);		// stereo in
	setNumOutputs (kNumOutputs);		// stereo out
	setUniqueID ('Dstn');	// identify
	canProcessReplacing ();	// supports replacing output
    
    
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
    
	
	IGainValue = (float) 0.0;	// input filter gain, dB
	IGainKnob = value2knob(IGainValue, IGainLimits, IGainTaper);
	
	OGainValue = (float) 0.0;	// output filter gain, dB
	OGainKnob = value2knob(OGainValue, OGainLimits, OGainTaper);
    
	FcValue = (float) 1000.0;	// input filter center frequency, Hz
	FcKnob = value2knob(FcValue, FcLimits, FcTaper);
    
	QValue = (float) 5.0;	// input filter resonance, ratio
	QKnob = value2knob(QValue, QLimits, QTaper);
	
	RRateValue = (float) 1.0;
	RRateKnob = (float) value2knob(RRateValue, RRateLimits, RRateTaper);
	
	ARateValue = (float) 1.0;
	ARateKnob = (float) value2knob(ARateValue, ARateLimits, ARateTaper);
	
	DepthValue = (float) 1.0;
	DepthKnob  = (float) value2knob(DepthValue, DepthLimits, DepthTaper);
    
	inGain = 1.0;
	outGain = 1.0;
	
    
    
	// signal processing parameter and state initialization
	fs = getSampleRate();	// sampling rate, Hz
	
	// Track the signal
	signalTracker.SetTauRelease(RRateValue, fs); // set the signal tracker
	signalTracker.SetTauAttack(ARateValue, fs); // set the signal tracker
	
    
	// place holder
	designResonantLowPass(lpCoefs, FcValue, QValue);
	lpFilter.SetCoefs(lpCoefs);
}

//-------------------------------------------------------------------------------------------------------
AutoWah::~AutoWah ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void AutoWah::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void AutoWah::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void AutoWah::setParameter(VstInt32 index, float value)
{
    
	// assign control and signal processing parameter values
	switch (index)
	{
            
        case kParamIGain:
            // input filter gain, dB
            IGainKnob = value;
            IGainValue = knob2value(IGainKnob, IGainLimits, IGainTaper);
			
            // input gain, amplitude
            inGain = pow(10.0, IGainValue/20.0);
            
            break;
        case kParamOGain:
            // input filter gain, dB
            OGainKnob = value;
            OGainValue = knob2value(OGainKnob, OGainLimits, OGainTaper);
            
            // input gain, amplitude
            outGain = pow(10.0, OGainValue/20.0);
            
            break;
            
        case kParamFc:
            // input filter center frequency, Hz
            FcKnob = value;
            FcValue = knob2value(FcKnob, FcLimits, FcTaper);
            
            break;
            
        case kParamQ:
            // input filter resonance, ratio
            QKnob = value;
            QValue = knob2value(QKnob, QLimits, QTaper);
            
            break;
        case kParamRRate:
            // input filter resonance, ratio
            RRateKnob = value;
            RRateValue = knob2value(RRateKnob, RRateLimits, RRateTaper);
            break;
        case kParamARate:
            // input filter resonance, ratio
            ARateKnob = value;
            ARateValue = knob2value(ARateKnob, ARateLimits, ARateTaper);
            break;
        case kParamDepth:
            // input filter resonance, ratio
            DepthKnob = value;
            DepthValue = knob2value(DepthKnob, DepthLimits, DepthTaper);
			
            break;			
        default :
            break;
	}
    
}


//-----------------------------------------------------------------------------------------
float AutoWah::getParameter(VstInt32 index)
{
	// return knob position
	switch (index)
	{
        case kParamIGain:
            // input filter gain, dB
            return IGainKnob;
            break;
        case kParamOGain:
            // input filter gain, dB
            return OGainKnob;
            break;
            
        case kParamFc:
            // input filter center frequency, Hz
            return FcKnob;
            break;
            
        case kParamQ:
            // input filter resonance, ratio
            return QKnob;
            break;
        case kParamRRate:
            // input filter resonance, ratio
            return RRateKnob;
            break;
        case kParamARate:
            // input filter resonance, ratio
            return ARateKnob;
            break;
        case kParamDepth:
            // input filter resonance, ratio
            return DepthKnob;
            break;
			
        default:
            return 0.0;
	}
}

//-----------------------------------------------------------------------------------------
void AutoWah::getParameterName(VstInt32 index, char *label)
{
	// return knob name
	switch (index)
	{
        case kParamIGain:
            // input filter gain, dB
            vst_strncpy(label, " I Gain ", kVstMaxParamStrLen);
            break;
        case kParamOGain:
            // input filter gain, dB
            vst_strncpy(label, " O Gain ", kVstMaxParamStrLen);
            break;
			
        case kParamFc:
            // input filter center frequency, Hz
            vst_strncpy(label, " Fc ", kVstMaxParamStrLen);
            break;
            
        case kParamQ:
            // input filter resonance, ratio
            vst_strncpy(label, " Q ", kVstMaxParamStrLen);
            break;
        case kParamRRate:
            // input filter resonance, ratio
            vst_strncpy(label, " Release Rate ", kVstMaxParamStrLen);
            break;
        case kParamARate:
            // input filter resonance, ratio
            vst_strncpy(label, " Attack Rate ", kVstMaxParamStrLen);
            break;
        case kParamDepth:
            // input filter resonance, ratio
            vst_strncpy(label, " Depth ", kVstMaxParamStrLen);
            break;
        default :
            *label = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void AutoWah::getParameterDisplay(VstInt32 index, char *text)
{
	// return parameter value as text
	switch (index)
	{
        case kParamIGain:
            // input filter gain, dB
            float2string(IGainValue, text, kVstMaxParamStrLen);
            break;
        case kParamOGain:
            // input filter gain, dB
            float2string(OGainValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamFc:
            // input filter center frequency, Hz
            float2string(FcValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamQ:
            // input filter resonance, ratio
            float2string(QValue, text, kVstMaxParamStrLen);
            break;
			
        case kParamRRate:
            // input filter resonance, ratio
            float2string(RRateValue, text, kVstMaxParamStrLen);
            break;
        case kParamARate:
            // input filter resonance, ratio
            float2string(ARateValue, text, kVstMaxParamStrLen);
            break;
			
        case kParamDepth:
            // input filter resonance, ratio
            float2string(DepthValue, text, kVstMaxParamStrLen);
            break;
        default :
            *text = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void AutoWah::getParameterLabel(VstInt32 index, char *label)
{
	switch (index)
	{
        case kParamIGain:
            // input filter gain, dB
            vst_strncpy(label, " dB ", kVstMaxParamStrLen);
            break;
        case kParamOGain:
            // input filter gain, dB
            vst_strncpy(label, " dB ", kVstMaxParamStrLen);
            break;
        case kParamFc:
            // input filter center frequency, Hz
            vst_strncpy(label, " Hz ", kVstMaxParamStrLen);
            break;
            
        case kParamQ:
            // input filter resonance, ratio
            vst_strncpy(label, " Q ", kVstMaxParamStrLen);
            break;
			
        case kParamRRate:
            // input filter resonance, ratio
            vst_strncpy(label, " Sec ", kVstMaxParamStrLen);
            break;
        case kParamARate:
            // input filter resonance, ratio
            vst_strncpy(label, " Sec ", kVstMaxParamStrLen);
            break;
        case kParamDepth:
            // input filter resonance, ratio
            vst_strncpy(label, " Ratio ", kVstMaxParamStrLen);
            break;
            
        default :
            *label = '\0';
            break;
	};
}

//------------------------------------------------------------------------
bool AutoWah::getEffectName (char* name)
{
	vst_strncpy (name, "AutoWah", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool AutoWah::getProductString (char* text)
{
	vst_strncpy (text, "AutoWah", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool AutoWah::getVendorString (char* text)
{
	vst_strncpy (text, "Stanford/CCRMA MUS424", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 AutoWah::getVendorVersion ()
{ 
	return 1000; 
}



//-----------------------------------------------------------------------------------------
void AutoWah::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
// overwrite output
{
    
	float*	in0 = inputs[0];
	float*  in1 = inputs[1];
	float*	out0 = outputs[0];
	float*  out1 = outputs[1];
    
	double track = 0; // tracked signal
	int i;
    
	for (i = 0; i < sampleFrames; i++)
	{
		// assign input
		double inp0 = (*in0);
		double inp1 = (*in1);
		double signal = inp0 + inp1;
		
		signal *= inGain;
        
		// Track the signal
		signalTracker.SetTauRelease(RRateValue, fs); // set the signal tracker
		signalTracker.SetTauAttack(ARateValue, fs); // set the signal tracker
        
		signalTracker.Process(signal, track);
		
		// modulation signal
		double modulationSignal = frequencyComputer(track);		
		
		// design new input filter
		designResonantLowPass(lpCoefs, modulationSignal, QValue);
		
		// set the filter stucture coefs
		lpFilter.SetCoefs(lpCoefs);
		
		// apply input gain, input filter
		lpFilter.Process(signal, signal);
		
		// apply gain, assign output
		*out0++ = signal*outGain;
		*out1++ = signal*outGain;
		
		// update input pointers
		in0++;in1++;
		
	}
}

//-----------------------------------------------------------------------------------------
void AutoWah::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
    double* in1  =  inputs[0];
    double* in2  =  inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];
    
    while (--sampleFrames >= 0)
    {
        (*out1++) = (*in1++);
        (*out2++) = (*in2++);
    }
}

//-----------------------------------------------------------------------------------------
float AutoWah::knob2value(float knob, const float *limits, float taper)
// convert knob on [0,1] to value in [limits[0],limits[1]] according to taper
{
	float value;
    
	if (taper > 0.0) {
		// algebraic taper
		value = limits[0] + (limits[1] - limits[0]) * pow(knob, taper);
	} else {
		// exponential taper
		value = limits[0] * exp(log(limits[1]/limits[0]) * knob);
	}
    
	return value;
};

//-----------------------------------------------------------------------------------------
float AutoWah::value2knob(float value, const float *limits, float taper)
// convert value in [limits[0],limits[1]] to knob on [0,1] according to taper
{
	float knob;
    
	if (taper > 0.0) {
		// algebraic taper
		knob = pow((value - limits[0])/(limits[1] - limits[0]), 1.0/taper);
	} else {
		// exponential taper
		knob = log(value/limits[0])/log(limits[1]/limits[0]);
	}
    
	return knob;
};


float AutoWah::frequencyComputer(float track)
{
	///////////////START//////////////////
  	
    double logDepth = log(DepthValue);
    double expTrack = exp(track * logDepth);
    
    double cutoff_freq = FcValue * expTrack;
	
	////////////////END/////////////////////
	
	return cutoff_freq;
}

void AutoWah::bilinearTranform(double acoefs[], double dcoefs[], double center)
{
	double b0, b1, b2, a0, a1, a2;		//storage for continuous-time filter coefs
	double bz0, bz1, bz2, az0, az1, az2;	// coefs for discrete-time filter.
	
	// For easier looking code...unpack
	b0 = acoefs[0]; b1 = acoefs[1]; b2 = acoefs[2]; a0 = acoefs[3]; a1 = acoefs[4]; a2 = acoefs[5];
	
	
	// TODO: apply bilinear transform
	///////////////START//////////////////
  	
	//bz0 = 1.0; bz1 = 0.0; bz2 = 0.0; az0 = 1.0; az1 = 0.0; az2 = 0.0;
	
//    double T = 1.0 / fs;
//    double wcT = center * 2 * pi * T;
//    double c = cos(wcT / 2) / sin(wcT / 2);
//    
//    bz2 = b2 * pow(c, 2) - b1 * c + b0;
//    bz1 = -2 * b2 * pow(c, 2) + 2 * b0;
//    bz0 = b2 * pow(c, 2) + b1 * c + b0;
//    
//    az2 = a2 * pow(c, 2) - a1 * c + a0;
//    az1 = -2 * a2 * pow(c, 2) + 2 * a0;
//    az0 = a2 * pow(c, 2) + a1 * c + a0;
//    
//    bz2 /= az0;
//    bz1 /= az0;
//    bz0 /= az0;
//    
//    az2 /= az0;
//    az1 /= az0;

    // For some reason my filter code started to blow up at high Q values, so I talked with Chris Carlson and we worked out the correct implementation specific to this filter form.
    // (Previously I'd only been using general biquad bilinear transforms.)
    // This is his code which I used after I worked it all out and understood what was going on.
    
    double T,Tsq, fb2,tTb1,b0Tsq,fa2,tTa1,a0Tsq,denomdiv;
    
    //compute quantities used more than once
    T = 1/fs;
    Tsq = T*T;
    fb2 = 4*b2;
    tTb1 = 2*T*b1;
    b0Tsq = b0*Tsq;
    fa2 = 4*a2;
    tTa1 = 2*T*a1;
    a0Tsq = a0*Tsq;
    
    //compute digital coeffs
    denomdiv = 1/(fa2 + tTa1 + a0Tsq);
    bz0 = (fb2 + tTb1 + b0Tsq)*denomdiv;
    az0 = 1;
    bz1 = (-8*b2 + 2*b0Tsq)*denomdiv;
    az1 = (-8*a2 + 2*a0Tsq)*denomdiv;
    bz2 = (fb2  - tTb1 + b0Tsq)*denomdiv;
    az2 = (fa2  - tTa1 + a0Tsq)*denomdiv;
    
    
	////////////////END/////////////////////
	
	// return coefficients to the output
	dcoefs[0] = bz0; dcoefs[1] = bz1; dcoefs[2] = bz2; dcoefs[3] = az1; dcoefs[4] = az2;
    
}

//-----------------------------------------------------------------------------------------
void AutoWah::designResonantLowPass(double* dcoefs, double center, double qval)
// design parametric filter based on input center frequency, gain, Q and sampling rate
{
	double b0, b1, b2, a0, a1, a2;		//storage for continuous-time filter coefs
	double acoefs[6];
    
	//Design parametric filter here. Filter should be of the form
	//
	//    2
	// b2s  + b1s + b0
	// ---------------
	//    2
	// a2s  + a1s + a0
	//
	// Parameters are center frequency in Hz, gain in dB, and Q.
	
	
	//TODO: design analog filter based on input gain, center frequency and Q	
	b0 = 1.0; b1 = 0.0; b2 = 0.0; a0 = 1.0; a1 = 0.0; a2 = 0.0;
 	///////////////START//////////////////
	
    //double rad_center = 1.0;
	//a1 = 1 / (qval * rad_center);
    //a2 = 1 / pow(rad_center, 2);
	
    double rad_center = center * 2 * pi;
    b0 = 1.0; b1 = 0.0; b2 = 0.0; a0 = 1.0; a1 = 1 / (qval * center); a2 = 1.0 / (center * center);
    
	////////////////END/////////////////////	
	// pack the analog coeffs into an array and apply the bilinear tranform
	acoefs[0] = b0; acoefs[1] = b1; acoefs[2] = b2; acoefs[3] = a0; acoefs[4] = a1; acoefs[5] = a2;
	
	// inputs the 6 analog coeffs, output the 5 digital coeffs
	bilinearTranform(acoefs, dcoefs, center);
	
}


