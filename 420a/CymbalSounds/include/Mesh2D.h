#ifndef STK_MESH2D_H
#define STK_MESH2D_H

#include "Instrmnt.h"
#include "OnePole.h"
#include "DelayL.h"
#include "NonLinearAllPass.h"


namespace stk {
    
    /***************************************************/
    /*! \class Mesh2D
     \brief Two-dimensional rectilinear waveguide mesh class.
     
     This class implements a rectilinear,
     two-dimensional digital waveguide mesh
     structure.  For details, see Van Duyne and
     Smith, "Physical Modeling with the 2-D Digital
     Waveguide Mesh", Proceedings of the 1993
     International Computer Music Conference.
     
     This is a digital waveguide model, making its
     use possibly subject to patents held by Stanford
     University, Yamaha, and others.
     
     Control Change Numbers: 
     - X Dimension = 2
     - Y Dimension = 4
     - Mesh Decay = 11
     - X-Y Input Position = 1
     
     by Julius Smith, 2000 - 2002.
     Revised by Gary Scavone for STK, 2002.
     */
    /***************************************************/
    
    const short NXMAX = 120;
    const short NYMAX = 120;
    
    class Mesh2D : public Instrmnt
    {
    public:
        //! Class constructor, taking the x and y dimensions in samples.
        Mesh2D( short nX, short nY );
        
        //! Class destructor.
        ~Mesh2D( void );
        
        //! Reset and clear all internal state.
        void clear( void );
        
        //void setDelays( StkFloat delay);
        
        //! Set the x dimension size in samples.
        void setNX( short lenX );
        
        //! Set the y dimension size in samples.
        void setNY( short lenY );
        
        //! Set the x, y input position on a 0.0 - 1.0 scale.
        void setInputPosition( StkFloat xFactor, StkFloat yFactor );
        
        //! Set the loss filters gains (0.0 - 1.0).
        void setDecay( StkFloat decayFactor );
        
        void noteOn( StkFloat frequency, StkFloat amplitude );
        
        //! Impulse the mesh with the given amplitude (frequency ignored).
        void noteOn( StkFloat frequency, StkFloat amplitude, StkFloat brightness );
        
        //! Stop a note with the given amplitude (speed of decay) ... currently ignored.
        void noteOff( StkFloat amplitude );
        
        //! Calculate and return the signal energy stored in the mesh.
        StkFloat energy( void );
        
        //! Input a sample to the mesh and compute one output sample.
        StkFloat inputTick( StkFloat input );
        
        //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
        void controlChange( int number, StkFloat value );
        
        //! Compute and return one output sample.
        StkFloat tick( unsigned int channel = 0 );
        
    protected:
        
        StkFloat tick0();
        StkFloat tick1();
        void clearMesh();
        
        short NX_, NY_;
        short xInput_, yInput_;
        NonLinearAllPass  filterX_[NXMAX];
        NonLinearAllPass  filterY_[NYMAX];
        StkFloat v_[NXMAX-1][NYMAX-1]; // junction velocities

        
        
//        DelayL vxp_[NXMAX][NYMAX];   // positive-x velocity wave
//        DelayL vxm_[NXMAX][NYMAX];   // negative-x velocity wave
//        DelayL vyp_[NXMAX][NYMAX];   // positive-y velocity wave
//        DelayL vym_[NXMAX][NYMAX];   // negative-y velocity wave
//        
//        // Alternate buffers
//        DelayL vxp1_[NXMAX][NYMAX];  // positive-x velocity wave
//        DelayL vxm1_[NXMAX][NYMAX];  // negative-x velocity wave
//        DelayL vyp1_[NXMAX][NYMAX];  // positive-y velocity wave
//        DelayL vym1_[NXMAX][NYMAX];  // negative-y velocity wave

        
        
        StkFloat vxp_[NXMAX][NYMAX];   // positive-x velocity wave
        StkFloat vxm_[NXMAX][NYMAX];   // negative-x velocity wave
        StkFloat vyp_[NXMAX][NYMAX];   // positive-y velocity wave
        StkFloat vym_[NXMAX][NYMAX];   // negative-y velocity wave
        
        // Alternate buffers
        StkFloat vxp1_[NXMAX][NYMAX];  // positive-x velocity wave
        StkFloat vxm1_[NXMAX][NYMAX];  // negative-x velocity wave
        StkFloat vyp1_[NXMAX][NYMAX];  // positive-y velocity wave
        StkFloat vym1_[NXMAX][NYMAX];  // negative-y velocity wave
        
        int counter_; // time in samples
    };
    
} // stk namespace

#endif
