//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : AutoWah.h
// Created by   : music424 staff
// Company      : CCRMA - Stanford
// Description  : 
//
// Date         : 5/16/11
//-------------------------------------------------------------------------------------------------------

#ifndef __AutoWah__
#define __AutoWah__

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef max
#define max(a,b)			(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif

#define kMaxLen			32

#define pi 3.14159265358979
#define eps 2.220446049250313e-16

// signal processing functions
struct Biquad {
	//  biquad filter section
	double	b0, b1, b2, a1, a2, z1, z2;
	
	Biquad() {
		this->b0=1.0;
		this->b1=0.0;
		this->b2=0.0;
		this->a1=0.0;
		this->a2=0.0;
		Reset();
	}
	void SetCoefs(double* coefs) {
		// set filter coefficients [b0 b1 b2 a1 a2]
		this->b0=*(coefs);
		this->b1=*(coefs+1);
		this->b2=*(coefs+2);
		this->a1=*(coefs+3);
		this->a2=*(coefs+4);
	}
	void Reset() {
		// reset filter state
		z1=0;
		z2=0;
	}
	void Process (double input, double& output) {
		// process input sample, direct form II transposed
		output = z1 + input*b0;
		z1 = z2 + input*b1 - output*a1;
		z2 = input*b2 - output*a2;
	}
};


// signal processing functions
struct SignalTracker {
	
    static const int num_past_inputs = 200;
    double past_inputs[num_past_inputs];
    int input_index;
    
	double	b0_r, a1_r, b0_a, a1_a, levelEstimate;
	
	SignalTracker() {
		
		// default pass through
		this->a1_r = 0; // release coeffs
		this->b0_r = 1; 
		this->a1_a = 0; // attach coeffs
		this->b0_a = 1;
		Reset();
	}
	
	void SetTauRelease(double tauRelease, double fs) {
		
		///////////////START//////////////////
		
        double n = tauRelease * fs;
        // lambda ^ n = 1 / e
        double lambda = exp(-1/n);
        
        this->b0_r = 1 - lambda;
        this->a1_r = lambda;
        
		////////////////END/////////////////////
		
	}
	void SetTauAttack(double tauAttack, double fs) {
		
		///////////////START//////////////////

        double n = tauAttack * fs;
        // lambda ^ n = 1 / e
        double lambda = exp(-1/n);
        
        this->b0_a = 1 - lambda;
        this->a1_a = lambda;
		
		////////////////END/////////////////////
	}
	
	void Reset() {
		// reset filter state
		levelEstimate=0;
        input_index = 0;
        for (int i = 0; i < num_past_inputs; i++)
        {
            past_inputs[i] = 0;
        }
	}
	
    double max_input()
    {
        double maximum = 0;
        for (int i = 0; i < num_past_inputs; i++)
        {
            if (past_inputs[i] > maximum)
            {
                maximum = past_inputs[i];
            }
        }
        return maximum;
    }
    
	void Process (double input, double& output) {
		
		///////////////START//////////////////
		
        double abs_input = fabs(input);
        past_inputs[input_index] = abs_input;
        input_index++;
        input_index %= num_past_inputs;
        double new_max = max_input();
        
        if (levelEstimate > new_max)
        {
            // attack mode
            output = abs_input * b0_a + a1_a * levelEstimate;
            levelEstimate = output;
        } else
        {
            // release mode
            output = abs_input * b0_r + a1_r * levelEstimate;
            levelEstimate = output;
        }
        
		////////////////END/////////////////////
	}
};


// signal processing functions
struct SlewedParameter {
	
	double	b0, a1, z1;
	
	SlewedParameter() {
		
		// default pass through
		this->a1 = 0;
		this->b0 = 1;
		Reset();
	}
	
	void SetTau(double tau, double fs) {
		
		///////////////START//////////////////
        
        double n = tau * fs;
        // lambda ^ n = 1 / e
        double lambda = exp(-1/n);
        
        this->b0 = 1 - lambda;
        this->a1 = lambda;
		
		////////////////END/////////////////////
	}
	
	void Reset() {
		// reset filter state
		z1=0;
	}
	void Process (double input, double& output) {
		///////////////START//////////////////
        output = input * b0 + a1 * z1;
        z1 = output;
		////////////////END/////////////////////
		
	}
};




//-------------------------------------------------------------------------------------------------------
class AutoWah : public AudioEffectX
{
public:
	AutoWah (audioMasterCallback audioMaster);
	~AutoWah ();
    
	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames);
    
	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
    
	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);
    
	void bilinearTranform(double acoefs[], double dcoeffs[], double center);
	void designResonantLowPass(double* dcoefs, double center, double qval);
	float frequencyComputer(float track);
    
	float knob2value(float knob, const float *limits, float taper);
	float value2knob(float value, const float *limits, float taper);
    
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
    
    
protected:
	char	programName[kVstMaxProgNameLen + 1];
    
	// configuration
	enum { 
		kNumProgs	= 1,
		kNumInputs	= 2,
		kNumOutputs	= 2
	};
    
	// user interface parameters
	enum {
		kParamIGain,
		kParamFc,
		kParamQ,
		kParamRRate,
		kParamARate,
		kParamDepth,
		kParamOGain,
		kNumParams
	};
    
    
	float IGainKnob, IGainValue;	// filter gain, dB
	float OGainKnob, OGainValue;	// filter gain, dB
	float FcKnob, FcValue;	// filter center frequency, Hz
	float QKnob, QValue;	// filter resonance, ratio
	float inGain, outGain;
	
	float RRateKnob, RRateValue;
	float ARateKnob, ARateValue;
	
	float DepthKnob, DepthValue;
    
	// signal processing parameters and state
	double fs;	// sampling rate, Hz
	
	SignalTracker signalTracker;
	
	double lpCoefs[5];	// filter coefficients
	Biquad lpFilter;		// filter
    
};


// input filter gain limits, dB; taper, exponent
const static float IGainLimits[2] = {-30.0, 30.0};
const static float IGainTaper = 1.0;

// input filter gain limits, dB; taper, exponent
const static float OGainLimits[2] = {-30.0, 30.0};
const static float OGainTaper = 1.0; 

// input filter center frequency limits, hz; taper, exponent
const static float FcLimits[2] = {50.0, 5000.0};
const static float FcTaper = -1.0;

// input filter resonance limits, dB; taper, exponent
const static float QLimits[2] = {0.25, 32.0};
const static float QTaper = -1.0;


const static float RRateLimits[2] = {0.001, 1.0};
const static float RRateTaper = -1.0;

const static float ARateLimits[2] = {0.001, 1.0};
const static float ARateTaper = -1.0;

const static float DepthLimits[2] = {0.01, 100.0};
const static float DepthTaper = -1.0;



////////////////END/////////////////////



#endif	// __AutoWah__


