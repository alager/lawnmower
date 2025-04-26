#include <cmath>
using std::abs;
#include <numeric>
#include <chrono>
#include <thread>
#include <pigpio.h>

#include "d2a.h"
#include "a2d.h"

// debug outputs
#define DEBUG_VOLTAGE		false
#define DEBUG_CURRENT		false

#define A2D_ENABLE			true
#define DAC_ENABLE			true

// Feedback gpio inputs
#define FEEDBACK_LEFT_MTR	( 5 )
#define FEEDBACK_RIGHT_MTR	( 2 )
#define FEEDBACK_CTR_MTR	( 17 )

#define ENABLE_MOTOR_GPIO	( 25 )

#define INPUT_GLITCH_FLTR	( 1 )

#define MIN_SPEED			( 16.0f )
#define MIN_SPEED_R			( 18.0f )
#define MIN_SPEED_L			( 16.0f )

#define MAX_SPEED			( 100.0f )

#define TLM_TOTAL_AMPS		( 0 )
#define TLM_LEFT_AMPS		( 1 )
#define TLM_RIGHT_AMPS		( 2 )
#define TLM_CUTR_AMPS		( 3 )
#define TLM_VOLTS			( 4 )

#define TIMER_0				( 0 )
#define MILLY_SECS_10		( 10 )
#define MILLY_SECS_100		( 100 )

#define LEFT				( 0 )
#define RIGHT				( 1 )

#pragma once
class Motors
{
public:
	
	typedef struct {
		float LSpeed;
		float RSpeed;
	} Speed;


	// Constructor
	Motors()
	{
		// Initialize Pi GPIO
		if ( gpioInitialise() < 0 ) 
			throw std::runtime_error( "failed to construct: GPIOInit" );

		
		#if A2D_ENABLE
		// create a a2d object
		a2d_ = new A2d();
		#endif

		#if DAC_ENABLE
		// get the private d2a object to control the motors
		d2a_= new D2a();
		#endif

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

		// turn the motor enables off
		motorEnable( false );

		std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));

		// turn the motor enables on
		motorEnable( true );

	}

	// Destructor
	~Motors()
	{
		estop();
	}

	static void speedTick( Motors *myObj );
	static void tickCallback( void *myObjV );
	static void gpioTick( Motors *myObj );
	static void tickA2D( Motors *myObj );

	static void internalTick( const gpioSample_t *samples, int numSamples, void *myObj );


	void init( void );
	void forward( float Lspeed, float Rspeed );
	void estop( void );
	void motorEnable( bool enable );
	float getSpeed( int leftRight );


private:
	D2a* d2a_;
	A2d* a2d_;
	uint8_t telemIdx_ = 0;
};
