//-------------------------------------------------------------------------------------------------------
// VST WahWah Plug-In
//
// Filename     : WahWah.cpp
// 
//-------------------------------------------------------------------------------------------------------

#include "WahWah.h"
#include <math.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new WahWah (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
WahWah::WahWah (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, kNumProgs, kNumParams)	// 1 program, 1 parameter only
{
	setNumInputs (kNumInputs);		// stereo in
	setNumOutputs (kNumOutputs);		// stereo out
	setUniqueID ('Dstn');	// identify
	canProcessReplacing ();	// supports replacing output


	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name

	
	GainValue = (float) 0.0;	// input filter gain, dB
	GainKnob = value2knob(GainValue, GainLimits, GainTaper);

	FcValue = (float) 1000.0;	// input filter center frequency, Hz
	FcKnob = value2knob(FcValue, FcLimits, FcTaper);

	QValue = (float) 5.0;	// input filter resonance, ratio
	QKnob = value2knob(QValue, QLimits, QTaper);
	
	RateValue = (float) 1.0;
	RateKnob = (float) value2knob(RateValue, RateLimits, RateTaper);
	
	DepthValue = (float) 1.0;
	DepthKnob  = (float) value2knob(DepthValue, DepthLimits, DepthTaper);

	drive = 1.0;

    // signal processing parameter and state initialization
    
    // In the starter code this call was being performed after the SetTau calls,
    // which use fs. That's an error that has been there before my time, but I 
    // failed to catch it and fix it! (jorgeh, Spring 2011)
	fs = getSampleRate();	// sampling rate, Hz
    
	fcSlewer.SetTau(.02, fs); // set the mod signal slewer
	gSlewer.SetTau(.02, fs); // set the mod signal slewer	
	qSlewer.SetTau(.02, fs); // set the mod signal slewer
	modSlewer.SetTau(.02, fs); // set the mod signal slewer

    
	// place holder
	designResonantLowPass(lpCoefs, FcValue, QValue);
	lpFilter.SetCoefs(lpCoefs);
}

//-------------------------------------------------------------------------------------------------------
WahWah::~WahWah ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void WahWah::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void WahWah::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void WahWah::setParameter(VstInt32 index, float value)
{

	// assign control and signal processing parameter values
	switch (index)
	{

	case kParamGain:
		// input filter gain, dB
		GainKnob = value;
		GainValue = knob2value(GainKnob, GainLimits, GainTaper);
			
		// input gain, amplitude
		drive = pow(10.0, GainValue/20.0);

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
	case kParamRate:
		// input filter resonance, ratio
		RateKnob = value;
		RateValue = knob2value(RateKnob, RateLimits, RateTaper);
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
float WahWah::getParameter(VstInt32 index)
{
	// return knob position
	switch (index)
	{
	case kParamGain:
		// input filter gain, dB
		return GainKnob;
		break;

	case kParamFc:
		// input filter center frequency, Hz
		return FcKnob;
		break;

	case kParamQ:
		// input filter resonance, ratio
		return QKnob;
		break;
	case kParamRate:
		// input filter resonance, ratio
		return RateKnob;
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
void WahWah::getParameterName(VstInt32 index, char *label)
{
	// return knob name
	switch (index)
	{
	case kParamGain:
		// input filter gain, dB
		vst_strncpy(label, " Gain ", kVstMaxParamStrLen);
		break;

	case kParamFc:
		// input filter center frequency, Hz
		vst_strncpy(label, " Fc ", kVstMaxParamStrLen);
		break;

	case kParamQ:
		// input filter resonance, ratio
		vst_strncpy(label, " Q ", kVstMaxParamStrLen);
		break;
	case kParamRate:
		// input filter resonance, ratio
		vst_strncpy(label, " Rate ", kVstMaxParamStrLen);
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
void WahWah::getParameterDisplay(VstInt32 index, char *text)
{
	// return parameter value as text
	switch (index)
	{
	case kParamGain:
		// input filter gain, dB
		float2string(GainValue, text, kVstMaxParamStrLen);
		break;

	case kParamFc:
		// input filter center frequency, Hz
		float2string(FcValue, text, kVstMaxParamStrLen);
		break;

	case kParamQ:
		// input filter resonance, ratio
		float2string(QValue, text, kVstMaxParamStrLen);
		break;
			
	case kParamRate:
		// input filter resonance, ratio
		float2string(RateValue, text, kVstMaxParamStrLen);
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
void WahWah::getParameterLabel(VstInt32 index, char *label)
{
	switch (index)
	{
	case kParamGain:
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
			
	case kParamRate:
		// input filter resonance, ratio
		vst_strncpy(label, " Hz ", kVstMaxParamStrLen);
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
bool WahWah::getEffectName (char* name)
{
	vst_strncpy (name, "WahWah", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool WahWah::getProductString (char* text)
{
	vst_strncpy (text, "WahWah", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool WahWah::getVendorString (char* text)
{
	vst_strncpy (text, "Stanford/CCRMA MUS424", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 WahWah::getVendorVersion ()
{ 
	return 1000; 
}

float WahWah::frequencyComputer(float fc, float rate, float depth)
{
	float floSignal = pow(depth, .5*LFO(rate));
	
	// Slew the modulation signal as well
	modSlewer.Process(floSignal, floSignal);
	
	return fc*floSignal;
	
}


//-----------------------------------------------------------------------------------------
void WahWah::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
// overwrite output
{

	float*	in0 = inputs[0];
	float*  in1 = inputs[1];
	float*	out0 = outputs[0];
	float*  out1 = outputs[1];

	int i;

	for (i = 0; i < sampleFrames; i++)
	{
		// assign input
		double inp0 = (*in0);
		double inp1 = (*in1);
		double signal = inp0 + inp1;
        
        float fc, d, q;
        
		// slew control signals
        /*
        // ORIGINAL
		fcSlewer.Process(FcValue, FcValue);
		gSlewer.Process(GainValue, GainValue);
		qSlewer.Process(QValue, QValue);
        */
        // SOLUTION
		fcSlewer.Process(FcValue, fc);
		gSlewer.Process(drive, d);
		qSlewer.Process(QValue, q);
		
		// modulation signal
        // ORIGINAL
		//float modulationSignal  = frequencyComputer(FcValue, RateValue, DepthValue);
        // SOLUTION
		float modulationSignal  = frequencyComputer(fc, RateValue, DepthValue);
		
		// design new input filter
        // ORIGINAL
		//designResonantLowPass(lpCoefs, modulationSignal, QValue);
        // SOLUTION
		designResonantLowPass(lpCoefs, modulationSignal, q);
		
		// set the filter stucture coefs
		lpFilter.SetCoefs(lpCoefs);
		
		// apply input gain, input filter
		lpFilter.Process(signal, signal);
		
		// apply gain, assign output
        // ORIGINAL
		//*out0++ = signal*drive;
		//*out1++ = signal*drive;
        // SOLUTION
        *out0++ = signal*d;
        *out1++ = signal*d;
		
		// update input pointers
		in0++;in1++;
		
	}
}

//-----------------------------------------------------------------------------------------
void WahWah::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
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
float WahWah::knob2value(float knob, const float *limits, float taper)
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
float WahWah::value2knob(float value, const float *limits, float taper)
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

float WahWah::LFO(float f0)
{
	///////////////START//////////////////
	static double phase = 0.0;
    phase += 2 * pi * f0 / fs;
    if (phase > 2 * pi) phase -= 2 * pi;	
	return sin( phase );
	////////////////END/////////////////////
	
}

void WahWah::bilinearTranform(double acoefs[], double dcoefs[])
{
	double b0, b1, b2, a0, a1, a2;		//storage for continuous-time filter coefs
	double bz0, bz1, bz2, az0, az1, az2;	// coefs for discrete-time filter.
	
	// For easier looking code...unpack
	b0 = acoefs[0]; b1 = acoefs[1]; b2 = acoefs[2]; a0 = acoefs[3]; a1 = acoefs[4]; a2 = acoefs[5];
		
	// TODO: apply bilinear transform
	///////////////START//////////////////
    
    double T = 1/fs;
    double Tsq = T*T;

    // we need to normalize because the biquad struct assumes az0 = 1
 
    az0 = ( a0*Tsq + 2*a1*T + 4*a2 );
    az1 = ( 2*a0*Tsq - 8*a2 ) / az0; 
    az2 = ( a0*Tsq - 2*a1*T + 4*a2 ) / az0;

	bz0 = ( b0*Tsq + 2*b1*T + 4*b2 ) / az0; 
    bz1 = ( 2*b0*Tsq - 8*b2 ) / az0;
    bz2 = ( b0*Tsq - 2*b1*T + 4*b2 ) / az0; 
    
    az0 = 1;
    
	////////////////END/////////////////////
	
	// return coefficients to the output
	dcoefs[0] = bz0; dcoefs[1] = bz1; dcoefs[2] = bz2; dcoefs[3] = az1; dcoefs[4] = az2;

}

//-----------------------------------------------------------------------------------------
void WahWah::designResonantLowPass(double* dcoefs, double center, double qval)
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
 	///////////////START//////////////////

    center *= 2*pi;
    
    b0 = 1.0; 
    b1 = 0.0; 
    b2 = 0.0; 
    
    a0 = 1.0; 
    a1 = 1.0 / ( center * qval ); 
    a2 = 1.0 / ( center * center );

	////////////////END/////////////////////	
	// pack the analog coeffs into an array and apply the bilinear tranform
	acoefs[0] = b0; acoefs[1] = b1; acoefs[2] = b2; acoefs[3] = a0; acoefs[4] = a1; acoefs[5] = a2;
	
	// inputs the 6 analog coeffs, output the 5 digital coeffs
	bilinearTranform(acoefs, dcoefs);
	
}




