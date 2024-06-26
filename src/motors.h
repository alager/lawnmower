#include "d2a.h"

// Feedback gpio inputs
#define FEEDBACK_LEFT_MTR	( 5 )
#define FEEDBACK_RIGHT_MTR	( 2 )
#define FEEDBACK_CTR_MTR	( 17 )

#define INPUT_GLITCH_FLTR	( 1 )

#pragma once
class Motors
{
public:

	// Constructor
	Motors()
	{
		// Initialize Pi GPIO
		if ( gpioInitialise() < 0 ) 
			throw std::runtime_error( "failed to construct: GPIOInit" );

		// get the private d2a object to control the motors
		d2a_= new D2a();

		// setup the feedback inputs
		gpioSetMode( FEEDBACK_LEFT_MTR, PI_INPUT);
		gpioSetMode( FEEDBACK_RIGHT_MTR, PI_INPUT);
		gpioSetMode( FEEDBACK_CTR_MTR, PI_INPUT);

		// deglitch the feedback inputs for 1us
		gpioGlitchFilter( FEEDBACK_LEFT_MTR, INPUT_GLITCH_FLTR );
		gpioGlitchFilter( FEEDBACK_RIGHT_MTR, INPUT_GLITCH_FLTR );
		gpioGlitchFilter( FEEDBACK_CTR_MTR, INPUT_GLITCH_FLTR );

		

	}

	// Destructor
	~Motors()
	{

	}

	void init( void );
	static void tick( void );
	static void internalTick( const gpioSample_t *samples, int numSamples );
	void forward( float speed );
	void estop( void );

private:
	D2a* d2a_;

};
