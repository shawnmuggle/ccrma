//-------------------------------------------------------------------------------------------------------
// VST Effect Plug-in
//
// Filename     : Reverb.h
// Created by   : jorgeh
// Company      : Stanford
// Description  : 
//
// Date         : 5/3/11
//-------------------------------------------------------------------------------------------------------

#ifndef __Reverb__
#define __Reverb__

#include "public.sdk/source/vst2.x/audioeffectx.h"

#ifndef max
#define max(a,b)			(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif

#define kMaxLen			32


//-------------------------------------------------------------------------------------------------------
class Reverb : public AudioEffectX
{
public:
	Reverb (audioMasterCallback audioMaster);
	~Reverb ();
    
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
    
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
    
	void designShelf(double* pcofs, long theLength, double transition, double T60low, double T60high);
    
    
protected:
	// param IDs
	enum {
		kParamT60low	= 0,
		kParamT60high,
		kParamTransition,
		kParamWetDry,
		kNumParams
	};
    
    
	// knob vars
	float T60LowKnob;
	float T60HighKnob;
	float TransitionKnob;
	float WetDryKnob;
	
	// config
	enum { 
		kNumProgs	= 1,
		kNumInputs	= 2,
		kNumOutputs	= 2
	};
    
	char	programName[kVstMaxProgNameLen + 1];
    
	struct HalfBiquad {			//  one-pole, one-zero shelf filter
		double	a1, b0, b1, z1;
		
		HalfBiquad()		{	this->a1=0.0; this->b0=1.0; this->b1=0.0; Reset();	}
		void	SetCoefs (double* coefs)		//pointer to array: [b0 b1 b2 a1 a2]
        {	this->a1=*(coefs+2); this->b0=*(coefs); this->b1=*(coefs+1);}
		void	Reset()	{	z1=0; }
		void	Process (double input, double& output)
		{
			//Transposed Direct II Form (PREFERRED)
			output = z1+input*b0;
			z1=input*b1-output*a1;
            
		}
	};
    
#define kMaxDelay 8192
	struct DelayLine {			// delay line
		double	dly[kMaxDelay];							// double-precision delay line
		long	wp,rp,theDelay;							// read, write pointers and delay length
		DelayLine()		{
			int indx;
			for (indx=0; indx<kMaxDelay ; indx++){
				dly[indx]=0.0;							// clear delay line on instantiation
			}
			wp=0;rp=0;									// init write and read pointers to zero
		}
		void SetDelay(long aDelay)
        {	theDelay=aDelay;}
		void Write(double data)
		{	dly[wp]=data;}								// write data into line
		double Read()
		{	return dly[rp];}							// read data from line
		void UpdatePointers()							// advance read, write pointers
		{	wp--;
			if(wp<0)
				wp=kMaxDelay-1;
			rp=wp+theDelay;
			if(rp>kMaxDelay-1)
				rp-=kMaxDelay;
			if(rp>kMaxDelay-1)
				rp=kMaxDelay-1;
			if(rp<0)
				rp=0;
		}
	};
    
    
	// internal state var declaration and initialization
	double the_sample_rate;
#define kNumDelays	12
	HalfBiquad fbfilt[kNumDelays];
	DelayLine dl[kNumDelays];
	double coefs[3];
	double*	pcoefs;
	double t60low,t60high,tFreq;
    
    
};

const static float InVecL[kNumDelays]={1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0};
const static float InVecR[kNumDelays]={0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0};
const static float OutVecL[kNumDelays]={1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0};
const static float OutVecR[kNumDelays]={0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0,0.0,1.0};
const static float dlens[kNumDelays]={2023,2153,2291,2438,2595,2761,2939,3127,3328,3542,3769,4011};

/*
 //Random Mixing Matrix
 const static double FB[kNumDelays][kNumDelays]={	
 
 {0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,    0.2887,},
 {0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,},
 {0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,},
 {0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,},
 {0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,},
 {0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,},
 {0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,},
 {0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,},
 {0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,    0.2887,},
 {0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,},
 {0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,    0.2887,},
 {0.2887,    0.2887,   -0.2887,    0.2887,    0.2887,    0.2887,   -0.2887,   -0.2887,   -0.2887,    0.2887,   -0.2887,   -0.2887,},
 
 };
 */

const static double FB[kNumDelays][kNumDelays]={	
    //Identity Matrix
    {+1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, +0.00000, },
    {+0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +0.00000, +1.00000, },
    
    
    
};

#endif	// __Reverb__
