#include <math.h>
#include <pigpio.h>

#include "d2a.h"
#include "a2d.h"

// Feedback gpio inputs
#define FEEDBACK_LEFT_MTR	( 5 )
#define FEEDBACK_RIGHT_MTR	( 2 )
#define FEEDBACK_CTR_MTR	( 17 )

#define ENABLE_MOTOR_GPIO	( 25 )

#define INPUT_GLITCH_FLTR	( 1 )

#define MIN_SPEED			( 16.0f )
#define MAX_SPEED			( 100.0f )

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
		// d2a_= new D2a();

		// create a a2d object
		a2d_ = new A2d();

		// setup the feedback inputs
		gpioSetMode( FEEDBACK_LEFT_MTR, PI_INPUT);
		gpioSetMode( FEEDBACK_RIGHT_MTR, PI_INPUT);
		gpioSetMode( FEEDBACK_CTR_MTR, PI_INPUT);

		// deglitch the feedback inputs for 1us
		gpioGlitchFilter( FEEDBACK_LEFT_MTR, INPUT_GLITCH_FLTR );
		gpioGlitchFilter( FEEDBACK_RIGHT_MTR, INPUT_GLITCH_FLTR );
		gpioGlitchFilter( FEEDBACK_CTR_MTR, INPUT_GLITCH_FLTR );

		// set the motor enable GPIO as an output
		gpioSetMode( ENABLE_MOTOR_GPIO, PI_OUTPUT );

		motorEnable( true );

	}

	// Destructor
	~Motors()
	{
		estop();
	}

	static void tick( Motors *myObj );
	static void tickA2D( Motors *myObj );

	static void internalTick( const gpioSample_t *samples, int numSamples, void *myObj );
	// static void internalTick( const gpioSample_t *samples, int numSamples );

	void init( void );
	void forward( float speed );
	void estop( void );
	void motorEnable( bool enable );


private:
	D2a* d2a_;
	A2d* a2d_;
	
};
