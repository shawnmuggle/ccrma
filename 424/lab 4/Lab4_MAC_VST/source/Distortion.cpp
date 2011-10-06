//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : Distortion.cpp
// Created by   : music424 staff
// Company      : CCRMA - Stanford University
// Description  : 
//
// Date         : 5/10/11
//-------------------------------------------------------------------------------------------------------

#include "Distortion.h"
#include <math.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new Distortion (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
Distortion::Distortion (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, kNumProgs, kNumParams)	// 1 program, 1 parameter only
{
	setNumInputs (kNumInputs);		// stereo in
	setNumOutputs (kNumOutputs);		// stereo out
	setUniqueID ('Dstn');	// identify
	canProcessReplacing ();	// supports replacing output
    
    
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
    
	// user interface parameter initialization
	DriveValue = (float) 0.0;	// input gain, dB
	DriveKnob = value2knob(DriveValue, DriveLimits, DriveTaper);
	drive = pow(10.0, DriveValue/20.0);
    
	LevelValue = (float) 0.0;	// input gain, dB
	LevelKnob = value2knob(LevelValue, LevelLimits, LevelTaper);
	level = pow(10.0, LevelValue/20.0);
    
	GainInValue = (float) 0.0;	// input filter gain, dB
	GainInKnob = value2knob(GainInValue, GainInLimits, GainInTaper);
    
	FcInValue = (float) 1000.0;	// input filter center frequency, Hz
	FcInKnob = value2knob(FcInValue, FcInLimits, FcInTaper);
    
	QInValue = (float) 5.0;	// input filter resonance, ratio
	QInKnob = value2knob(QInValue, QInLimits, QInTaper);
    
	GainOutValue = (float) 0.0;	// input filter gain, dB
	GainOutKnob = value2knob(GainOutValue, GainOutLimits, GainOutTaper);
    
	FcOutValue = (float) 1000.0;	// input filter center frequency, Hz
	FcOutKnob = value2knob(FcOutValue, FcOutLimits, FcOutTaper);
    
	QOutValue = (float) 5.0;	// input filter resonance, ratio
	QOutKnob = value2knob(QOutValue, QOutLimits, QOutTaper);
    
    
	// signal processing parameter and state initialization
	fs = getSampleRate();	// sampling rate, Hz
    
	designParametric(InCoefs, FcInValue, GainInValue, QInValue);
	InFilter.SetCoefs(InCoefs);
    
	designParametric(OutCoefs, FcOutValue, GainOutValue, QOutValue);
	OutFilter.SetCoefs(OutCoefs);
    
	for (int j = 0; j < kAAOrder; j++) {
		AIFilter[j].SetCoefs((double *) AACoefs[j]);
		AAFilter[j].SetCoefs((double *) AACoefs[j]);
	}
    
}

//-------------------------------------------------------------------------------------------------------
Distortion::~Distortion ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void Distortion::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Distortion::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Distortion::setParameter(VstInt32 index, float value)
{
    
	// assign control and signal processing parameter values
	switch (index)
	{
        case kParamDrive:
            // input gain, dB
            DriveKnob = value;
            DriveValue = knob2value(DriveKnob, DriveLimits, DriveTaper);
            
            // input gain, amplitude
            drive = pow(10.0, DriveValue/20.0);
            
            break;
            
        case kParamLevel:
            // output gain, dB
            LevelKnob = value;
            LevelValue = knob2value(LevelKnob, LevelLimits, LevelTaper);
            
            // input gain, amplitude
            level = pow(10.0, LevelValue/20.0);
            
            break;
            
        case kParamGainIn:
            // input filter gain, dB
            GainInKnob = value;
            GainInValue = knob2value(GainInKnob, GainInLimits, GainInTaper);
            
            // design new input filter
            designParametric(InCoefs, FcInValue, GainInValue, QInValue);
            InFilter.SetCoefs(InCoefs);
            
            break;
            
        case kParamFcIn:
            // input filter center frequency, Hz
            FcInKnob = value;
            FcInValue = knob2value(FcInKnob, FcInLimits, FcInTaper);
            
            // design new input filter
            designParametric(InCoefs, FcInValue, GainInValue, QInValue);
            InFilter.SetCoefs(InCoefs);
            
            break;
            
        case kParamQIn:
            // input filter resonance, ratio
            QInKnob = value;
            QInValue = knob2value(QInKnob, QInLimits, QInTaper);
            
            // design new input filter
            designParametric(InCoefs, FcInValue, GainInValue, QInValue);
            InFilter.SetCoefs(InCoefs);
            
            break;
            
        case kParamGainOut:
            // output filter gain, dB
            GainOutKnob = value;
            GainOutValue = knob2value(GainOutKnob, GainOutLimits, GainOutTaper);
            
            // design new output filter
            designParametric(OutCoefs, FcOutValue, GainOutValue, QOutValue);
            OutFilter.SetCoefs(OutCoefs);
            
            break;
            
        case kParamFcOut:
            // output filter center frequency, Hz
            FcOutKnob = value;
            FcOutValue = knob2value(FcOutKnob, FcOutLimits, FcOutTaper);
            
            // design new output filter
            designParametric(OutCoefs, FcOutValue, GainOutValue, QOutValue);
            OutFilter.SetCoefs(OutCoefs);
            
            break;
            
        case kParamQOut:
            // output filter resonance, ratio
            QOutKnob = value;
            QOutValue = knob2value(QOutKnob, QOutLimits, QOutTaper);
            
            // design new output filter
            designParametric(OutCoefs, FcOutValue, GainOutValue, QOutValue);
            OutFilter.SetCoefs(OutCoefs);
            
            break;
            
        default :
            break;
	}
    
}


//-----------------------------------------------------------------------------------------
float Distortion::getParameter(VstInt32 index)
{
	// return knob position
	switch (index)
	{
        case kParamDrive:
            // input gain, dB
            return DriveKnob;
            break;
            
        case kParamLevel:
            // output gain, dB
            return LevelKnob;
            break;
            
        case kParamGainIn:
            // input filter gain, dB
            return GainInKnob;
            break;
            
        case kParamFcIn:
            // input filter center frequency, Hz
            return FcInKnob;
            break;
            
        case kParamQIn:
            // input filter resonance, ratio
            return QInKnob;
            break;
            
        case kParamGainOut:
            // output filter gain, dB
            return GainOutKnob;
            break;
            
        case kParamFcOut:
            // output filter center frequency, Hz
            return FcOutKnob;
            break;
            
        case kParamQOut:
            // output filter resonance, ratio
            return QOutKnob;
            break;
            
        default:
            return 0.0;
	}
}

//-----------------------------------------------------------------------------------------
void Distortion::getParameterName(VstInt32 index, char *label)
{
	// return knob name
	switch (index)
	{
        case kParamDrive:
            // input gain, dB
            vst_strncpy(label, " Input Gain ", kVstMaxParamStrLen);
            break;
            
        case kParamLevel:
            // input gain, dB
            vst_strncpy(label, " Output Gain ", kVstMaxParamStrLen);
            break;
            
        case kParamGainIn:
            // input filter gain, dB
            vst_strncpy(label, " Input Filter Gain ", kVstMaxParamStrLen);
            break;
            
        case kParamFcIn:
            // input filter center frequency, Hz
            vst_strncpy(label, " Input Filter Fc ", kVstMaxParamStrLen);
            break;
            
        case kParamQIn:
            // input filter resonance, ratio
            vst_strncpy(label, " Input Filter Q ", kVstMaxParamStrLen);
            break;
            
        case kParamGainOut:
            // output filter gain, dB
            vst_strncpy(label, " Output Filter Gain ", kVstMaxParamStrLen);
            break;
            
        case kParamFcOut:
            // output filter center frequency, Hz
            vst_strncpy(label, " Output Filter Fc ", kVstMaxParamStrLen);
            break;
            
        case kParamQOut:
            // output filter resonance, ratio
            vst_strncpy(label, " Output Filter Q ", kVstMaxParamStrLen);
            break;
            
        default :
            *label = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void Distortion::getParameterDisplay(VstInt32 index, char *text)
{
	// return parameter value as text
	switch (index)
	{
        case kParamDrive:
            // input gain, dB
            float2string(DriveValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamLevel:
            // output gain, dB
            float2string(LevelValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamGainIn:
            // input filter gain, dB
            float2string(GainInValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamFcIn:
            // input filter center frequency, Hz
            float2string(FcInValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamQIn:
            // input filter resonance, ratio
            float2string(QInValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamGainOut:
            // output filter gain, dB
            float2string(GainOutValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamFcOut:
            // output filter center frequency, Hz
            float2string(FcOutValue, text, kVstMaxParamStrLen);
            break;
            
        case kParamQOut:
            // output filter resonance, ratio
            float2string(QOutValue, text, kVstMaxParamStrLen);
            break;
            
        default :
            *text = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void Distortion::getParameterLabel(VstInt32 index, char *label)
{
	switch (index)
	{
        case kParamDrive:
            // input gain, dB
            vst_strncpy(label, " dB ", kVstMaxParamStrLen);
            break;
            
        case kParamLevel:
            // output gain, dB
            vst_strncpy(label, " dB ", kVstMaxParamStrLen);
            break;
            
        case kParamGainIn:
            // input filter gain, dB
            vst_strncpy(label, " dB ", kVstMaxParamStrLen);
            break;
            
        case kParamFcIn:
            // input filter center frequency, Hz
            vst_strncpy(label, " Hz ", kVstMaxParamStrLen);
            break;
            
        case kParamQIn:
            // input filter resonance, ratio
            vst_strncpy(label, "    ", kVstMaxParamStrLen);
            break;
            
        case kParamGainOut:
            // output filter gain, dB
            vst_strncpy(label, " dB ", kVstMaxParamStrLen);
            break;
            
        case kParamFcOut:
            // output filter center frequency, Hz
            vst_strncpy(label, " Hz ", kVstMaxParamStrLen);
            break;
            
        case kParamQOut:
            // output filter resonance, ratio
            vst_strncpy(label, "    ", kVstMaxParamStrLen);
            break;
            
        default :
            *label = '\0';
            break;
	};
}

//------------------------------------------------------------------------
bool Distortion::getEffectName (char* name)
{
	vst_strncpy (name, "Distortion", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool Distortion::getProductString (char* text)
{
	vst_strncpy (text, "Distortion", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool Distortion::getVendorString (char* text)
{
	vst_strncpy (text, "Stanford/CCRMA MUS424", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 Distortion::getVendorVersion ()
{ 
	return 1000; 
}



//-----------------------------------------------------------------------------------------
void Distortion::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
// overwrite output
{
    
	float*	in0 = inputs[0];
	float*  in1 = inputs[1];
	float*	out0 = outputs[0];
	float*  out1 = outputs[1];
    
	double isignal, fsignal, osignal, usignal, dsignal;
    
	int i, j, k;
    
	for (i = 0; i < sampleFrames; i++)
	{
		// assign, format input
		//
        
		// assign input
		double inp0 = (*in0);
		double inp1 = (*in1);
        
		// add noise to input to prevent denormal
		float noise = ((float) (rand()-16384)) * 6.103515623e-015;
		isignal = inp0 + inp1 + noise;
        
        
        
		// apply input gain, input filter
		InFilter.Process(drive*isignal, fsignal);
		
        
		// upsample, apply distortion, downsample
		for (k = 0; k < kUSRatio; k++) {
			// upsample (insert zeros)
			usignal = (k == 0) ? fsignal : 0.0;
            
			// apply antiimaging filter
			for (j = 0; j < kAAOrder; j++) {
				AIFilter[j].Process(usignal,usignal);
			}
            
			// apply distortion
			// note: x / (1+|x|) gives a soft saturation
			// where as min(1, max(-1, x)) gives a hard clipping
			dsignal = usignal / (1.0 + fabs(usignal));
            //dsignal = min(1.0, max(-1.0, usignal));
            
			// apply antialiasing filter
			for (j = 0; j < kAAOrder; j++) {
				AAFilter[j].Process(dsignal,dsignal);
			}
            
		}
        
        
		// apply output gain, output filter
		OutFilter.Process(level*dsignal, osignal);
        
        
		// apply gain, assign output
		*out0++ = osignal;
		*out1++ = osignal;
        
		// update input pointers
		in0++;in1++;
	}
}

//-----------------------------------------------------------------------------------------
void Distortion::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
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
float Distortion::knob2value(float knob, const float *limits, float taper)
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
float Distortion::value2knob(float value, const float *limits, float taper)
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

void Distortion::bilinearTranform(double acoefs[], double dcoefs[], double center)
{
	double b0, b1, b2, a0, a1, a2;		//storage for continuous-time filter coefs
	double bz0, bz1, bz2, az0, az1, az2;	// coefs for discrete-time filter.
	
	// For easier looking code...unpack
	b0 = acoefs[0]; b1 = acoefs[1]; b2 = acoefs[2]; a0 = acoefs[3]; a1 = acoefs[4]; a2 = acoefs[5];
	
	
	// TODO: apply bilinear transform
	///////////////START//////////////////
    
	bz0 = 1.0; bz1 = 0.0; bz2 = 0.0; az0 = 1.0; az1 = 0.0; az2 = 0.0;
    	
    double T = 1.0 / fs;
    double wcT = center * 2 * pi * T;
    double c = cos(wcT / 2) / sin(wcT / 2);
    
    bz2 = b2 * pow(c, 2) - b1 * c + b0;
    bz1 = -2 * b2 * pow(c, 2) + 2 * b0;
    bz0 = b2 * pow(c, 2) + b1 * c + b0;
    
    az2 = a2 * pow(c, 2) - a1 * c + a0;
    az1 = -2 * a2 * pow(c, 2) + 2 * a0;
    az0 = a2 * pow(c, 2) + a1 * c + a0;
    
    bz2 /= az0;
    bz1 /= az0;
    bz0 /= az0;
    
    az2 /= az0;
    az1 /= az0;
	
	////////////////END/////////////////////
	
	// return coefficients to the output
	dcoefs[0] = bz0; dcoefs[1] = bz1; dcoefs[2] = bz2; dcoefs[3] = az1; dcoefs[4] = az2;
    
}

//-----------------------------------------------------------------------------------------
void Distortion::designParametric(double* dcoefs, double center, double gain, double qval)
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
	
    float linear_gain = pow(10, gain/20);
    
    if (linear_gain < 1.0)
    {
        
        b2 = 1.0;
        b1 = linear_gain / qval;
        
        a2 = 1.0;
        a1 = 1.0 / qval;
        
    }
    else
    {
        b2 = 1.0;
        b1 = 1.0 / qval;
        
        a2 = 1.0;
        a1 = 1.0 / (qval * linear_gain);
    }
    
	
	////////////////END/////////////////////	
	// pack the analog coeffs into an array and apply the bilinear tranform
	acoefs[0] = b0; acoefs[1] = b1; acoefs[2] = b2; acoefs[3] = a0; acoefs[4] = a1; acoefs[5] = a2;
	
	// inputs the 6 analog coeffs, output the 5 digital coeffs
	bilinearTranform(acoefs, dcoefs, center);
	
}

