//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : Reverb.cpp
// Created by   : music424 staff
// Company      : Stanford
// Description  : 
//
// Date         : 5/3/11
//-------------------------------------------------------------------------------------------------------

#include "Reverb.h"
#include <math.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new Reverb (audioMaster);
}

//-------------------------------------------------------------------------------------------------------
Reverb::Reverb (audioMasterCallback audioMaster)
: AudioEffectX (audioMaster, 1, kNumParams)	// 1 program, 1 parameter only
{
	setNumInputs (kNumInputs);		// stereo in
	setNumOutputs (kNumOutputs);		// stereo out
	setUniqueID ('Rvrb');	// identify
	canProcessReplacing ();	// supports replacing output
    
    
	vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
    
	// internal state var declaration and initialization
    
	the_sample_rate=getSampleRate();
	pcoefs = coefs;									// pointer for filter coef assignment
	t60low=3.5;				// time for low freqs to decay 60dB
	t60high=0.5;			// time for high freqs to decay 60dB
	tFreq=6700.0;			// transition freq between low and high freqs
	WetDryKnob=(float)0.083;		// output mix
	T60LowKnob=(float)0.35;		// knob for low freq T60
	T60HighKnob=(float)0.05;		// knob for high freq T60
	TransitionKnob=(float)0.84909279555045;	// knob for transition freq
	int(initind);							
	for(initind=0; initind<kNumDelays; initind++){		
		dl[initind].SetDelay(dlens[initind]);						// set reverb delay lengths
		designShelf(pcoefs,dlens[initind],tFreq,t60low,t60high);	// design filters for feedback loop
		fbfilt[initind].SetCoefs(coefs);							// assign filter coefs
	}
    
}

//-------------------------------------------------------------------------------------------------------
Reverb::~Reverb ()
{
	// nothing to do here
}

//-------------------------------------------------------------------------------------------------------
void Reverb::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Reverb::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Reverb::setParameter (VstInt32 index, float value)
{
	switch (index)
	{
            int anind;
        case kParamT60low:
            T60LowKnob=value;
            t60low=10.0*T60LowKnob;
            for (anind=0; anind<kNumDelays ; anind++){
                designShelf(pcoefs,dlens[anind],tFreq,t60low,t60high);
                fbfilt[anind].SetCoefs(coefs);
            }
            break;
        case kParamT60high:
            T60HighKnob=value;
            t60high=10.0*T60HighKnob;
            for (anind=0; anind<kNumDelays ; anind++){
                designShelf(pcoefs,dlens[anind],tFreq,t60low,t60high);
                fbfilt[anind].SetCoefs(coefs);
            }
            break;
        case kParamTransition:
            TransitionKnob=value;
            tFreq=50.0*pow(10.0,log10(16000.0/50.0)*TransitionKnob); //log-spaced freqs from 50-16k
            for (anind=0; anind<kNumDelays ; anind++){
                designShelf(pcoefs,dlens[anind],tFreq,t60low,t60high);
                fbfilt[anind].SetCoefs(coefs);
            }
            break;
        case kParamWetDry:
            WetDryKnob=value;
            break;
            
        default :
            break;
	};
}

//-----------------------------------------------------------------------------------------
float Reverb::getParameter (VstInt32 index)
{
	switch (index)
	{
        case kParamT60low:
            return T60LowKnob;
            break;
        case kParamT60high:
            return T60HighKnob;
            break;
        case kParamTransition:
            return TransitionKnob;
            break;
        case kParamWetDry:
            return WetDryKnob;
            break;
        default :
            return 0.0;
	};
}

//-----------------------------------------------------------------------------------------
void Reverb::getParameterName (VstInt32 index, char* label)
{
	switch (index)
	{
        case kParamT60low:
            vst_strncpy(label, " T60Low ", kVstMaxParamStrLen);
            break;
        case kParamT60high:
            vst_strncpy(label, " T60High ", kVstMaxParamStrLen);
            break;
        case kParamTransition:
            vst_strncpy(label, " Transition ", kVstMaxParamStrLen);
            break;
        case kParamWetDry:
            vst_strncpy(label, " Wet/Dry ", kVstMaxParamStrLen);
            break;
        default :
            *label = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void Reverb::getParameterDisplay (VstInt32 index, char* text)
{
	switch (index)
	{
        case kParamT60low:
            float2string(t60low, text, kVstMaxParamStrLen);
            break;
        case kParamT60high:
            float2string(t60high, text, kVstMaxParamStrLen);
            break;
        case kParamTransition:
            float2string(tFreq, text, kVstMaxParamStrLen);
            break;
        case kParamWetDry:
            float2string(100.0*WetDryKnob, text, kVstMaxParamStrLen);
            break;
        default :
            *text = '\0';
            break;
	};
}

//-----------------------------------------------------------------------------------------
void Reverb::getParameterLabel (VstInt32 index, char* label)
{
	switch (index)
	{
        case kParamT60low:
            vst_strncpy(label, " s ", kVstMaxParamStrLen);
            break;
        case kParamT60high:
            vst_strncpy(label, " s ", kVstMaxParamStrLen);
            break;
        case kParamTransition:
            vst_strncpy(label, " Hz ", kVstMaxParamStrLen);
            break;
        case kParamWetDry:
            vst_strncpy(label, " % ", kVstMaxParamStrLen);
            break;
        default :
            *label = '\0';
            break;
	};
}

//------------------------------------------------------------------------
bool Reverb::getEffectName (char* name)
{
	vst_strncpy (name, "Reverb", kVstMaxEffectNameLen);
	return true;
}

//------------------------------------------------------------------------
bool Reverb::getProductString (char* text)
{
	vst_strncpy (text, "Reverb", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
bool Reverb::getVendorString (char* text)
{
	vst_strncpy (text, "Stanford/CCRMA MUS424", kVstMaxVendorStrLen);
	return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 Reverb::getVendorVersion ()
{ 
	return 1000; 
}


//-----------------------------------------------------------------------------------------
void Reverb::designShelf(double* pcofs, long theLength, double transition, double t60low, double t60high)
{
	double a1,b0,b1,norm,b0z,b1z,a1z;
	double roundTrip=((double)(theLength))/the_sample_rate;		// get delay length in seconds
	double g0,g1;  //Temp shelf gains at DC and Nyquist
	//  Design Shelf filter here. Must produce 1-pole, 1-zero filter coefficients
	//
	//  b1s + b0
	//  ---------
	//  a1s + 1
	//
	//  where b0 = asymptotic shelf gain at low frequency,
	//  b1/a1 = asymptotic shelf gain at high frequency.
	//  These gains must be computed so that, for the
	//  given corresponding delay time T, the signal has
	//  been reduced by the factor exp(-T/tau), where tau
	//  is the natural time constant associated with the
	//  desired T60.
	//  It is ok either to put the pole of the filter at the
	//  transition frequency, or to make the transition
	//  frequency equal to the geometric mean of the pole
	//  and the zero. Explain what you are doing for full
	//  credit.
    
	b0 = 0.9;	// Dummy bypass filter, gain = 0.9
	b1 = 0.9;	// at all frequencies
	a1 = 1.0;
    
	// Bilinear transform to make discrete-time filter.
	// This version of the bilinear transform uses the
	// low-frequency approximation to map 's' onto 'z'.
	// For extra credit, replace with a version of the
	// transform which will match continuous- and
	// discrete- time frequencies at the transition frequency
	// of the filter.
    
	norm=1+a1*(2*the_sample_rate);			// do bilinear transform
	b0z=(b0+b1*(2*the_sample_rate))/norm;	// (bilinear)
	b1z=(b0-b1*(2*the_sample_rate))/norm;	// (bilinear)
	a1z=(1-a1*(2*the_sample_rate))/norm;	// (bilinear)
    
	*(pcoefs) = b0z;						// return coefs
	*(pcoefs+1) = b1z;
	*(pcoefs+2) = a1z;
    
}



//-----------------------------------------------------------------------------------------
void Reverb::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float*	in0		= inputs[0];
	float*  in1     = inputs[1];
	float*	out0	= outputs[0];
	float*  out1    = outputs[1];
    
	for (int i = 0; i < sampleFrames; i++)
	{
		int noise=rand()-16384;
		double inp0=(*in0)+(float)(noise)*6.103515623e-015;	// additive noise to prevent denormal
		double inp1=(*in1)+(float)(noise)*6.103515623e-015;	// problem on P4 (bad for reverbs).
		double accum,OutL,OutR;
		int dlind,sumind;
		OutL=0.0;OutR=0.0;
		for (dlind=0; dlind<kNumDelays ; dlind++)
		{
			accum=0.0;
			for (sumind=0; sumind<kNumDelays ; sumind++)		
			{
				accum+=FB[dlind][sumind]*dl[sumind].Read();			// add up contributions from each delay line through orthonormal matrix
			}
			OutL+=OutVecL[dlind]*accum; OutR+=OutVecR[dlind]*accum;	// sum into L,R busses
			accum+=InVecL[dlind]*inp0+InVecR[dlind]*inp1;			// add in L,R contributions for current line
			fbfilt[dlind].Process(accum,accum);						// filter data with shelf
			dl[dlind].Write(accum);									// write back into current delay line
		}
		for (dlind=0; dlind<kNumDelays ; dlind++)
			dl[dlind].UpdatePointers();								// advance read, write pointers
        
		*out0++ = OutL*WetDryKnob + inp0*(1.0-WetDryKnob);			// compute wet/dry output
		*out1++ = OutR*WetDryKnob + inp1*(1.0-WetDryKnob);
		in0++;in1++;
	}
}

//-----------------------------------------------------------------------------------------
void Reverb::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
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
