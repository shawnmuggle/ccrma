//-------------------------------------------------------------------------------------------------------
// VST WahWah Plug-In
//
// Filename     : WahWah.h
//
//-------------------------------------------------------------------------------------------------------

#ifndef __WahWah__
#define __WahWah__

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <math.h>

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
struct SlewedParameter {
	
	double	b0, a1, z1, lambda;
	
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
        lambda = exp(-1/n);
		
		///////////////END//////////////////
	}
	
	void Reset() {
		// reset filter state
		z1=0;
	}
	void Process (float input, float& output) {
		///////////////START//////////////////

        output = z1 * lambda + input * (1 - lambda);
        z1 = output;
		
		///////////////END//////////////////
	}
};




//-------------------------------------------------------------------------------------------------------
class WahWah : public AudioEffectX
{
public:
	WahWah (audioMasterCallback audioMaster);
	~WahWah ();

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
	float LFO(float f0);
	float frequencyComputer(float fc, float rate, float depth);

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
		kParamFc,
		kParamQ,
		kParamRate,
		kParamDepth,
		kParamGain,
		kNumParams
	};


	float GainKnob, GainValue;	// filter gain, dB
	float FcKnob, FcValue;	// filter center frequency, Hz
	float QKnob, QValue;	// filter resonance, ratio
	float drive;
	
	float RateKnob, RateValue;
	float DepthKnob, DepthValue;

	// signal processing parameters and state
	double fs;	// sampling rate, Hz
	
	SlewedParameter fcSlewer, gSlewer, qSlewer, modSlewer;
	
	
	double lpCoefs[5];	// filter coefficients
	Biquad lpFilter;		// filter
    
    double phase;

};


// input filter gain limits, dB; taper, exponent
const static float GainLimits[2] = {-24.0, 24.0};
const static float GainTaper = 1.0;

// input filter center frequency limits, hz; taper, exponent
const static float FcLimits[2] = {50.0, 5000.0};
const static float FcTaper = -1.0;

// input filter resonance limits, dB; taper, exponent
const static float QLimits[2] = {0.25, 32.0};
const static float QTaper = -1.0;

const static float RateLimits[2] = {0.01, 1000.0};
const static float RateTaper = -1.0;

const static float DepthLimits[2] = {0.01, 100.0};
const static float DepthTaper = -1.0;
 
 

////////////////END/////////////////////



#endif	// __WahWah_HPP


