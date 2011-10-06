//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : Compressor.h
// Created by   : music424 staff
// Company      : CCRMA - Stanford
// Description  : 
//
// Date         : 5/17/11
//-------------------------------------------------------------------------------------------------------

#ifndef __compressor__
#define __compressor__

#include "public.sdk/source/vst2.x/audioeffectx.h"

#ifndef max
#define max(a,b)			(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif

#define kMaxLen			32


//-------------------------------------------------------------------------------------------------------
class Compressor : public AudioEffectX
{
public:
	Compressor (audioMasterCallback audioMaster);
	~Compressor ();
    
	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
    
	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
    
	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);
    
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
    
protected:
	// param IDs
	enum {
		kParamInputGain	= 0,
		kParamThreshold,
		kParamAttack,
		kParamRelease,
		kParamRatio,
		kParamOutputGain,
		kParamGRMeter,
		kNumParams
	};
    
    
	// knob vars
	float InputGainKnob;
	float ThresholdKnob;
	float AttackKnob;
	float ReleaseKnob;
	float RatioKnob;
	float OutputGainKnob;
	
	// config
	enum { 
		kNumProgs	= 1,
		kNumInputs	= 2,
		kNumOutputs	= 2
	};
    
	char	programName[kVstMaxProgNameLen + 1];
    
    
	// internal state var declaration and initialization
	double the_sample_rate;
	double input_gain;
	double output_gain;
	double threshold;
	double logthresh;
	double attack_time;		// attack time in seconds
	double attack_amount;
	double release_time;		// release time in seconds
	double release_amount;
	double comp_ratio;		// post-knee compression ratio
	double loglev;			
	double filtstate;			// level estimate filter state storage.
	double gainval;
	double dbgainval;
};

#endif
