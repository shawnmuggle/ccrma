//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : Compressor.cpp
// Created by   : music424 staff
// Company      : CCRMA - Stanford
// Description  : 
//
// Date         : 5/17/11
//-------------------------------------------------------------------------------------------------------

#include "Compressor.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new Compressor (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
Compressor::Compressor (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, 1, kNumParams)	// 1 program, 1 parameter only
{
	setNumInputs (kNumInputs);		// stereo in
	setNumOutputs (kNumOutputs);		// stereo out
	setUniqueID ('Cmpr');	// identify
	canProcessReplacing ();	// supports replacing output
    
    
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
    
	// internal state var declaration and initialization
	the_sample_rate=getSampleRate();
	InputGainKnob = 0.5;
	OutputGainKnob = 0.5;
	ThresholdKnob = 0.75;
	AttackKnob = 0.0;
	ReleaseKnob = 0.125;
	RatioKnob = 1.0;
    
	input_gain=pow(10.0,(60.0*InputGainKnob-30.0)/20.0);
	output_gain=pow(10.0,(60.0*OutputGainKnob-30.0)/20.0);
	threshold=pow(10.0,(30.0*ThresholdKnob-30.0)/20.0);
	logthresh=20.0*log10(threshold);
    
	attack_time = pow(10.0,(AttackKnob*3.0-4.0));
	attack_amount=1.0-exp(-1.0/(attack_time*the_sample_rate));
	
	release_time = 0.070*pow(10.0,(log10(20.0)*ReleaseKnob));
	the_sample_rate=getSampleRate();
	release_amount=1.0-exp(-1.0/(release_time*the_sample_rate));
    
	comp_ratio = pow(10.0,(RatioKnob*3.0));
    
	loglev=-96.0;
	filtstate=threshold;
	gainval=1.0;
	dbgainval=0.0;
    
    //memset(past_inputs, 0, num_past_inputs * sizeof(double));
    
}

//-------------------------------------------------------------------------------------------------------
Compressor::~Compressor ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void Compressor::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Compressor::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Compressor::setParameter (VstInt32 index, float value)
{
	switch (index)
	{
        case kParamInputGain :
            InputGainKnob=value;
            input_gain=pow(10.0,(60.0*InputGainKnob-30.0)/20.0);
            break;
        case kParamThreshold:
            ThresholdKnob=value;
            threshold=pow(10.0,(30.0*ThresholdKnob-30.0)/20.0);
            logthresh=20.0*log10(threshold);
            break;
        case kParamAttack :
            AttackKnob = value;
            attack_time = pow(10.0,(AttackKnob*3.0-4.0));
            the_sample_rate=getSampleRate();
            attack_amount=1.0-exp(-1.0/(attack_time*the_sample_rate));
            break;
        case kParamRelease:
            ReleaseKnob = value;
            release_time = 0.0175*pow(10.0,(log10(80.0)*ReleaseKnob));
            the_sample_rate=getSampleRate();
            release_amount=1.0-exp(-1.0/(release_time*the_sample_rate));
            break;
        case kParamRatio:
            RatioKnob = value;
            comp_ratio = pow(10.0,(RatioKnob*3.0));
            break;
        case kParamOutputGain:
            OutputGainKnob=value;
            output_gain=pow(10.0,(60.0*OutputGainKnob-30.0)/20.0);
            break;
        default :
            break;
	}
}

//-----------------------------------------------------------------------------------------
float Compressor::getParameter (VstInt32 index)
{
	switch (index)
	{
        case kParamInputGain :
            return InputGainKnob;
            break;
        case kParamThreshold:
            return ThresholdKnob;
            break;
        case kParamAttack:
            return AttackKnob;
            break;
        case kParamRelease:
            return ReleaseKnob;
            break;
        case kParamRatio:
            return RatioKnob;
            break;
        case kParamOutputGain:
            return OutputGainKnob;
            break;
        case kParamGRMeter:
            return max(min(1.0,(dbgainval+20.0)/20.0),0.0);
            break;
        default :
            return 0.0;
	}
}

//-----------------------------------------------------------------------------------------
void Compressor::getParameterName (VstInt32 index, char* label)
{
	switch (index)
	{
        case kParamInputGain :
            vst_strncpy(label, "InGain",kVstMaxParamStrLen);
            break;
        case kParamThreshold:
            vst_strncpy(label, " Thresh ",kVstMaxParamStrLen);
            break;
        case kParamAttack:
            vst_strncpy(label, "At Time",kVstMaxParamStrLen);
            break;
        case kParamRelease:
            vst_strncpy(label, "Rel Time ",kVstMaxParamStrLen);
            break;
        case kParamRatio:
            vst_strncpy(label, "Ratio",kVstMaxParamStrLen);
            break;
        case kParamOutputGain:
            vst_strncpy(label, "OutGain",kVstMaxParamStrLen);
            break;
        case kParamGRMeter:
            vst_strncpy(label, "Gain Red",kVstMaxParamStrLen);
            break;
        default :
            *label = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void Compressor::getParameterDisplay (VstInt32 index, char* text)
{
	switch (index)
	{
        case kParamInputGain :
            float2string((float)20.0*log10(input_gain), text,kVstMaxParamStrLen);
            break;
        case kParamThreshold :
            float2string((float)logthresh, text,kVstMaxParamStrLen);
            break;
        case kParamAttack:
            float2string((float)1000.0*attack_time, text, kVstMaxParamStrLen);
            break;
        case kParamRelease:
            float2string((float)1000.0*release_time, text, kVstMaxParamStrLen);
            break;
        case kParamRatio:
            float2string((float)comp_ratio, text, kVstMaxParamStrLen);
            break;
        case kParamOutputGain:
            float2string((float)20.0*log10(output_gain), text, kVstMaxParamStrLen);
            break;
        case kParamGRMeter:
            float2string((float)dbgainval, text, kVstMaxParamStrLen);
            break;
        default :
            *text = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void Compressor::getParameterLabel (VstInt32 index, char* label)
{
	switch (index)
	{
        case kParamInputGain :
            vst_strncpy(label, "dB", kVstMaxParamStrLen);
            break;
        case kParamThreshold:
            vst_strncpy(label, "dB", kVstMaxParamStrLen);
            break;
        case kParamAttack:
            vst_strncpy(label, "mSec", kVstMaxParamStrLen);
            break;
        case kParamRelease:
            vst_strncpy(label, "mSec", kVstMaxParamStrLen);
            break;
        case kParamRatio:
            vst_strncpy(label, " ", kVstMaxParamStrLen);
            break;
        case kParamOutputGain:
            vst_strncpy(label, "dB", kVstMaxParamStrLen);
            break;
        case kParamGRMeter:
            vst_strncpy(label, "dB", kVstMaxParamStrLen);
            break;
        default :
            *label = '\0';
            break;
	};
}

//------------------------------------------------------------------------
bool Compressor::getEffectName (char* name)
{
	vst_strncpy (name, "Compressor", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool Compressor::getProductString (char* text)
{
	vst_strncpy (text, "Compressor", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool Compressor::getVendorString (char* text)
{
	vst_strncpy (text, "Stanford MUS424", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 Compressor::getVendorVersion ()
{ 
	return 1000; 
}

//-----------------------------------------------------------------------------------------
void Compressor::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];
    
	for (int i = 0; i < sampleFrames; i++)
	{
		///////////// Process Loop (sample by sample) //////////////
        
		// Apply Input Gain to Signal
		double inp0=(*in1)*input_gain;
		double inp1=(*in2)*input_gain;		
        
		///////////  SIGNAL DETECTOR  //////////////////////////
		// Find Highest level of current sample of the stereo signal
		double input=fabs(inp0);
		if (fabs(inp1)>fabs(inp0))
			input=fabs(inp1);
        
        ////////////////////////////////////////////////////////////////////////////
        // TODO: implement a signal level detector. The level should be stored in the 
        //       'filtstate' variable. For a peak detector use the provided 
        //       'release_amount' and 'attack_amount' variables
        ////////////////////////////////////////////////////////////////////////////
        //filtstate = maxval;

        if (input > filtstate)
        {
            // attack mode
            //filtstate = filtstate + (1 - attack_amount) * (input - filtstate);
            filtstate = (1 - attack_amount) * filtstate + attack_amount * input;
        } else
        {
            // release mode
            //filtstate = filtstate + (1 - release_amount) * (threshold - filtstate);
            filtstate = (1 - release_amount) * filtstate + release_amount * threshold;
        }
        
		/////////////  GAIN COMPUTER  ///////////////////
		// take signal level estimate into log domain
		if (filtstate<0.00001)				// if below -100dB, set to -100dB
			loglev=20.0*log10(0.00001);		// to prevent taking log of zero
		else
            loglev=20.0 * log10(filtstate);
        

        ////////////////////////////////////////////////////////////////////////////
        // TODO : implement the gain computer logic here
        ////////////////////////////////////////////////////////////////////////////
        dbgainval=0.0;
        
        if (loglev >= logthresh)
        {
            dbgainval = 20 * log10(pow(filtstate / threshold, 1 / comp_ratio - 1));
        }
        
        // compute gain for current ratio
		// Compute linear gain for compressor
		gainval=pow(10,(dbgainval/20.0));		
		
		//////// Apply compressor gain and output gain to signal
		*out1++ = inp0*gainval*output_gain;
		*out2++ = inp1*gainval*output_gain;
        
		// increment input buffer pointer for next sample
		in1++;in2++;
        
        
	}
    // This breaks stuff!
	//updateDisplay();
}
