#include "NonLinearAllPass.h"

namespace stk {
    
    NonLinearAllPass :: NonLinearAllPass( StkFloat a1, StkFloat a2 )
    {
        a1_ = a1;
        a2_ = a2;
        
        a_ = a2_;
        
        fRec_.resize(2, 1, 0.0);
        fVec_.resize(2, 1, 0.0);
        
        inputs_.resize( 1, 1, 0.0 );
        outputs_.resize( 2, 1, 0.0 );
        
        lpf_.setPole(0.0);
        lpf_.setGain(1.0);
        
        u_n_minus_1_ = 0;
    }
    
    NonLinearAllPass :: ~NonLinearAllPass()    
    {
    }
            
} // stk namespace
