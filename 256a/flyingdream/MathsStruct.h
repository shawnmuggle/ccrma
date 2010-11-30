/*	Structures definitions

*/

#ifndef __MATHSSTRUCT_H_
#define __MATHSSTRUCT_H_

//////////////////////////

const float PI       =  3.14159265358979323846f;		// Pi

#define	DEGTORAD(x)	( ((x) * PI) / 180.0 )
#define	RADTODEG(x)	( ((x) * 180.0) / PI )

#define	SQR(x)		( (x) * (x) )

// limits a value to low and high
#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}

inline void SWAP(float &x, float &y) {	float temp;	temp = x;	x = y;	y = temp;	}

////////////

// Floating Point Library Specific

const float	EPSILON						= 0.005f;		// error tolerance for check
const int	FLOAT_DECIMAL_TOLERANCE		= 3;			// decimal places for float rounding

#define ZERO_CLAMP(x)	( (EPSILON > fabs(x))?0.0f:(x) )						// set float to 0 if within tolerance

#define FLOAT_EQ(x,v)	( ((v) - EPSILON) < (x) && (x) < ((v) + EPSILON) )		// float equality test

///////////

inline float ROUND(const float value, const int accuracy)		// round a float to a specified degree of accuracy
{
	double integer, fraction;

	fraction = modf(value, &integer);		// get fraction and int components

	return(float(integer + (float(int(fraction*pow(10,accuracy)))) / pow(10, accuracy) ) );
}	// end float ROUND


///////////////////////////

#endif	// __MATHSSTRUCT_H_
