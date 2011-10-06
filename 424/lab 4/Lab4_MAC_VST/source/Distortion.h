//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : Distortion.h
// Created by   : music424 staff
// Company      : CCRMA - Stanford University
// Description  : 
//
// Date         : 5/10/11
//-------------------------------------------------------------------------------------------------------

#ifndef __Distortion__
#define __Distortion__

#include "public.sdk/source/vst2.x/audioeffectx.h"

#ifndef max
#define max(a,b)			(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif

#define kMaxLen			32

#define pi 3.14159265358979
#define eps 2.220446049250313e-16


//-------------------------------------------------------------------------------------------------------
class Distortion : public AudioEffectX
{
public:
	Distortion (audioMasterCallback audioMaster);
	~Distortion ();
    
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
	void designParametric(double* peqcofs, double center, double gain, double qval);
    
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
		kParamDrive,
		kParamLevel,
		kParamGainIn,
		kParamFcIn,
		kParamQIn,
		kParamGainOut,
		kParamFcOut,
		kParamQOut,
		kNumParams
	};
    
	float DriveKnob, DriveValue;	// input gain, dB
	float LevelKnob, LevelValue;	// output gain, dB
    
	float GainInKnob, GainInValue;	// input filter gain, dB
	float FcInKnob, FcInValue;	// input filter center frequency, Hz
	float QInKnob, QInValue;	// input filter resonance, ratio
    
	float GainOutKnob, GainOutValue;	// output filter gain, dB
	float FcOutKnob, FcOutValue;	// output filter center frequency, Hz
	float QOutKnob, QOutValue;	// output filter resonance, ratio
    
    
    
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
    
	// signal processing parameters and state
	double fs;	// sampling rate, Hz
    
	double drive, level;	// input, output gains, amplitude
    
	double InCoefs[5];	// input filter coefficients
	Biquad InFilter;	// input filter
    
	double OutCoefs[5];	// input filter coefficients
	Biquad OutFilter;	// output filter
    
	enum{kUSRatio = 8};	// upsampling factor, sampling rate ratio
	enum{kAAOrder = 6};	// antialiasing/antiimaging filter order, biquads
	Biquad AIFilter[kAAOrder];	// antiimaging filter
	Biquad AAFilter[kAAOrder];	// antialiasing filter
    
};


// input drive limits, dB; taper, exponent
const static float DriveLimits[2] = {-24.0, 24.0};
const static float DriveTaper = 1.0;

// output level limits, dB; taper, exponent
const static float LevelLimits[2] = {-48.0, 24.0};
const static float LevelTaper = 1.0;


// input filter gain limits, dB; taper, exponent
const static float GainInLimits[2] = {-24.0, 24.0};
const static float GainInTaper = 1.0;

// input filter center frequency limits, hz; taper, exponent
const static float FcInLimits[2] = {50.0, 5000.0};
const static float FcInTaper = -1.0;

// input filter resonance limits, dB; taper, exponent
const static float QInLimits[2] = {0.25, 32.0};
const static float QInTaper = -1.0;


// output filter gain limits, dB; taper, exponent
const static float GainOutLimits[2] = {-24.0, 24.0};
const static float GainOutTaper = 1.0;

// output filter center frequency limits, hz; taper, exponent
const static float FcOutLimits[2] = {50.0, 5000.0};
const static float FcOutTaper = -1.0;

// output filter resonance limits, dB; taper, exponent
const static float QOutLimits[2] = {0.25, 32.0};
const static float QOutTaper = -1.0;

// antialiasing/antiimaging filter coefficients
// Needs modification (cutoff freq, number of biquads)
// If number of biquads changed, make sure to change kAAOrder above.

// TODO: Design the Anti-alias/Anti-imaging filter in matlab and input here
///////////////START//////////////////

const static double AACoefs[6][5] = {
	//{0.2929,    1.0000,         0,    -0.4142,         0,},
    {0.0005,    0.0006,    0.0005,    -1.1018,    0.3088,},
    {1.0000,   -0.8958,    1.0000,    -1.2238,    0.4126,},
    {1.0000,   -1.4762,    1.0000,    -1.3961,    0.5606,},
    {1.0000,   -1.6740,    1.0000,    -1.5597,    0.7041,},
    {1.0000,   -1.7544,    1.0000,    -1.6969,    0.8293,},
    {1.0000,   -1.7850,    1.0000,    -1.8132,    0.9430,},
};


////////////////END/////////////////////



#endif	// __Distortion_HPP


