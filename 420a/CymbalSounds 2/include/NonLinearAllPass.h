#ifndef STK_NONLINEARALLPASS_H
#define STK_NONLINEARALLPASS_H

#include "Filter.h"
#include "OnePole.h"

namespace stk {
    
    class NonLinearAllPass : public Filter
    {
    public:
        
        //! The default constructor
        NonLinearAllPass( StkFloat a1 = 0.3, StkFloat a2 = 0.5);
        
        //! Class destructor.
        ~NonLinearAllPass();
        
        //! Set the b[0] coefficient value.
        void setA1( StkFloat a1 ) { a1_ = a1; };
        
        //! Set the a[1] coefficient value.
        void setA2( StkFloat a2 ) { a2_ = a2; };
        
        //! Return the last computed output value.
        StkFloat lastOut( void ) const { return lastFrame_[0]; };
        
        //! Input one sample to the filter and return one output.
        StkFloat tick( StkFloat input );
        
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        
        void setPole(StkFloat pole) { lpf_.setPole(pole); };
        void setGain(StkFloat gain) { lpf_.setGain(gain); };

    protected:
        
        StkFrames fRec_;
        StkFrames fVec_;
        
        StkFloat a1_;
        StkFloat a2_;
        
        StkFloat u_n_minus_1_;
        StkFloat a_;
        
        OnePole lpf_;
    };
    
    inline StkFloat NonLinearAllPass :: tick( StkFloat input )
    {
        float x_n = lpf_.tick(input);
        
        //float x_n = input;
        
        x_n = gain_ * x_n;

        float u_n = x_n - a_ * u_n_minus_1_;

        if (u_n > 0)
            a_ = a1_;
        else
            a_ = a2_;
        
        float y_n = a_ * u_n + u_n_minus_1_;

        u_n_minus_1_ = u_n;
        
        lastFrame_[0] = y_n;
        return lastFrame_[0];
    }
    
    inline StkFrames& NonLinearAllPass :: tick( StkFrames& frames, unsigned int channel )
    {
        return frames;
    }

        
} // stk namespace

#endif
