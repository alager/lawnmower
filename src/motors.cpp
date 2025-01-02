#include "motors.h"

static uint32_t g_mask;
static int g_num_gpios;
#define MAX_GPIOS 32
#define OPT_R_DEF 5

static int g_gpio[ MAX_GPIOS ];
static uint32_t reportInterval = OPT_R_DEF * 100000;	// 500ms default
static volatile int g_pulse_count[ MAX_GPIOS ];
static volatile int g_update_counts = 1;

// globals that allow the static callbacks to share data with the objects
float targetSpeed_A_, targetSpeed_B_;
float currentSpeed_A_, currentSpeed_B_;

// telemetry 
float Vbatt;		// Input Voltage
float AmpTotal;		// Total
float AmpMtrL; 		// Left
float AmpMtrR; 		// Right
float AmpMtrCtr;	// Cutter 

// #define ArraySz	( sizeof( VbattArry_ ) / sizeof( VbattArry_[ 0 ] ) )


void Motors::init( void )
{

	// these are the gpio to track
	g_gpio[ g_num_gpios++ ] = FEEDBACK_LEFT_MTR;
	g_gpio[ g_num_gpios++ ] = FEEDBACK_RIGHT_MTR;
	// g_gpio[ g_num_gpios++ ] = FEEDBACK_CTR_MTR;

	g_mask = ( 1 << FEEDBACK_LEFT_MTR | 1 << FEEDBACK_RIGHT_MTR /*| 1 << FEEDBACK_CTR_MTR */ );

	#if DAC_ENABLE
		// configure a callback for gpio changes based on the g_mask
		gpioSetGetSamplesFuncEx( Motors::internalTick, g_mask, this );
		cout << "motor feedback configured" << endl;
	#endif
	
	// configure a call back to happen every 100ms for A2D and Motor speed
	gpioSetTimerFuncEx( TIMER_0, MILLY_SECS_10, Motors::tickCallback, this );
	
}

void Motors::tickCallback( void *myObjV )
{
	static u_int16_t speedCount = 0;

	#if A2D_ENABLE
		tickA2D( static_cast< Motors* >( myObjV ) );
	#endif

	#if DAC_ENABLE
		if( speedCount++ > 50 )
		{
			speedCount = 0;
			speedTick( static_cast< Motors* >( myObjV ) );
		}
	#endif
}

// This gpioTick() is called about every ~500ms
// maintenance function to keep the motors going
// at the specified speed.
//  F=2*P/N*60
// where
// F = RPM
// N = number of poles in the motor
// P = freq. in Hz
#define POLES			( 8 )

// larger number make longer ramp
#define RAMP_RATE		( 4.0f )
void Motors::speedTick( Motors *myObj )
{
	// update current speed towards target speed
	if( currentSpeed_A_ != targetSpeed_A_ )
	{
		float delta = targetSpeed_A_ - currentSpeed_A_;

		if( abs( delta ) > 2.0f )
			currentSpeed_A_ += delta / RAMP_RATE;
		else
			currentSpeed_A_ = targetSpeed_A_;


		cout << "A CSpeed: " << currentSpeed_A_ << ", TSpeed: " << targetSpeed_A_ << endl;

		myObj->d2a_->set('a', currentSpeed_A_ );
	}

	if( currentSpeed_B_ != targetSpeed_B_ )
	{
		float delta = targetSpeed_B_ - currentSpeed_B_;

		if( abs( delta ) > 2.0f )
			currentSpeed_B_ += delta / RAMP_RATE;
		else
			currentSpeed_B_ = targetSpeed_B_;

		cout << "B CSpeed: " << currentSpeed_B_ << ", TSpeed: " << targetSpeed_B_ << endl;
		myObj->d2a_->set('b', currentSpeed_B_ );
	}
}



void Motors::gpioTick( Motors *myObj )
{
	float perSec;

	( void ) myObj;

	perSec = 10.0 / OPT_R_DEF;

	cout << "gpioTick: ";

	if (!g_update_counts)
	{
		for ( int i = 0; i < g_num_gpios; i++)
		{
			printf(" gpio %1d count = %2d", g_gpio[i], (int)(perSec * (float)g_pulse_count[i]));

			float rpm = (float)(2 * (int)(perSec * (float)g_pulse_count[i]) / POLES * 60);
			cout << " rpm: " << rpm << ", ";
		}

		cout << endl;
		g_update_counts = 1;
	}
	cout << "CSpeed: " << currentSpeed_A_ << ", TSpeed: " << targetSpeed_A_ << endl;
}


// The intrnal tick call back run by Pigpio.  It gets called based on gpio changes
void Motors::internalTick( const gpioSample_t *samples, int numSamples, void *myObj )
{
	static int inited = 0;

	static uint32_t lastLevel;
	static uint32_t lastReportTick;
	static int count[MAX_GPIOS];

	uint32_t high, level, tickDiff;
	int i, g;

	
	if (!inited)
	{
		cout << "\n\n\n***Initing***" << endl;
		cout << "g_num_gpios: " << g_num_gpios << endl;
		
		inited = 1;

		lastLevel = samples[0].level;
		lastReportTick = samples[0].tick;

		for (i=0; i<g_num_gpios; i++)
		{
			count[i] = 0;
		}
	}

	// this loop handles reading in the GPIO pulses in an efficient manner
	// instead of an interrupt for each one.
	for (int s = 0; s < numSamples; s++)
	{
		tickDiff = samples[s].tick - lastReportTick;
		if (tickDiff >= reportInterval)
		{
			lastReportTick = samples[s].tick;

			if (g_update_counts)
			{
				for (i=0; i < g_num_gpios; i++)
					g_pulse_count[i] = count[i];
				
				g_update_counts = 0;
				gpioTick( (Motors*)(myObj) );
			}

			for (i=0; i < g_num_gpios; i++) count[i] = 0;
		}

		level = samples[s].level;
		high = ( (lastLevel ^ level) & g_mask) & level;
		lastLevel = level;
		
		/* only interested in low to high */
		if (high)
		{
			for ( g=0; g<g_num_gpios; g++ )
			{
				if ( high & ( 1 << g_gpio[g] ) ) 
					count[g]++;
			}
		}
	}
}


// gets called ~10ms by the pigpio timer
void Motors::tickA2D( Motors *myObj )
{
	static bool first_LA = true;
	static bool first_RA = true;
	static bool first_CA = true;
	static bool first_TA = true;
	static bool first_V = true;

	// cast it from void to Motors type for the rest of the code
	// Motors *myObj = static_cast< Motors* >( myObjV );

	// read the A2D data
	float telem = myObj->a2d_->update( myObj->telemIdx_ );

	// cout << std::fixed << std::setprecision(4);

	switch( myObj->telemIdx_ )
	{
		case TLM_TOTAL_AMPS:
			// calibration
			telem -= 450.0f + 32.0f; // offset correction ** estimated **

			// circuit equation to get us back to voltage
			// 55mV/A = 18.181818A/mV
			telem = (( telem * ( 5.12f / static_cast<float>( pow( 2, 14 ) ) ) ) - 2.50f) * 18.181818f;

			if( first_TA )
			{
				first_TA = false;
				AmpTotal = telem;
			}
			AmpTotal += ( telem - AmpTotal) / 8.0f;
			#if DEBUG_CURRENT
			 cout << "Total Current:		" << AmpTotal << endl;
			#endif
			break;

		case TLM_LEFT_AMPS:
			// calibration
			telem -= 130.0f + 5.0f; // offset correction

			// circuit equation to get us back to voltage
			// 55mV/A = 18.181818A/mV
			telem = (( telem * ( 5.12f / static_cast<float>( pow( 2, 14 ) ) ) ) - 2.50f) * 18.181818f;

			if( first_LA )
			{
				first_LA = false;
				AmpMtrL = telem;
			}
			AmpMtrL += ( telem - AmpMtrL) / 8.0f;
			#if DEBUG_CURRENT
			 cout << "Left Current:		" << AmpMtrL << endl;
			#endif
			break;

		case TLM_RIGHT_AMPS:
			// calibration
			telem -= 450.0f + 5.0f; // offset correction

			// circuit equation to get us back to voltage
			// 55mV/A = 18.181818A/mV
			telem = (( telem * ( 5.12f / static_cast<float>( pow( 2, 14 ) ) ) ) - 2.50f) * 18.181818f;

			if( first_RA )
			{
				first_RA = false;
				AmpMtrR = telem;
			}
			AmpMtrR += ( telem - AmpMtrR) / 8.0f;
			#if DEBUG_CURRENT
			 cout << "Right Current:		" << AmpMtrR << endl;
			#endif
			break;

		case TLM_CUTR_AMPS:
			// calibration
			telem -= 288.0f - 2.0f; // offset correction

			// circuit equation to get us back to voltage
			// 55mV/A = 18.181818A/mV
			telem = (( telem * ( 5.12f / static_cast<float>( pow( 2, 14 ) ) ) ) - 2.50f) * 18.181818f;

			if( first_CA )
			{
				first_CA = false;
				AmpMtrCtr = telem;
			}
			AmpMtrCtr += ( telem - AmpMtrCtr) / 8.0f;

			// cout << "Cutter Current:		" << AmpMtrCtr << endl;
			break;

		case TLM_VOLTS:
			telem = telem * 1.06208566323f; // calibration

			// circuit equation to get us back to voltage
			// telem * 5.12 / 2^14 * 11.07 / 1.07
			telem = ( telem * ( 5.12f / static_cast<float>( pow( 2, 14 ) ) ) ) * 11.07f / 1.07f;
			// cout << ", VoltageTelem: " << telem;
			
			if( first_V )
			{
				first_V = false;
				Vbatt = telem;					
			}
			
			Vbatt += ( telem - Vbatt) / 8.0f;
			#if DEBUG_VOLTAGE
			 cout << "DEBUG: VoltageAvg:		" << Vbatt << endl;
			#endif
			break;

		default:
			break;
	}

	// increment to the next telemetry value 
	if( ++myObj->telemIdx_ > 4 )
		myObj->telemIdx_ = 0;

}


// set the forward speed (0-100%)
// minimum of 16% to get motor motion
void Motors::forward( float speed )
{
	// do range protections
	if( speed > MAX_SPEED )
		speed = MAX_SPEED;
	if( speed < 5.0f )
		speed = 0.0f;
	else
	if( speed < MIN_SPEED )
		speed = MIN_SPEED;

	targetSpeed_A_ = targetSpeed_B_ = speed;

	// left
	// if( currentSpeed_A_ < MIN_SPEED )
	// {
	// 	currentSpeed_A_ = MIN_SPEED;
	// 	d2a_->set( 'a', MIN_SPEED );
	// }

	// // right
	// if( currentSpeed_B_ < MIN_SPEED_R )
	// {
	// 	currentSpeed_B_ = MIN_SPEED_R;
	// 	d2a_->set( 'b', MIN_SPEED_R );
	// }
}


// emergency stop of the motors
void Motors::estop( void )
{
	// turn the motor enables off
	motorEnable( false );

	// set the dacs to 0% speed 
	d2a_->set( 'a', 0.0f );
	d2a_->set( 'b', 0.0f );
	d2a_->set( 'c', 0.0f );
}


void Motors::motorEnable( bool enable )
{
	if( enable )
		gpioWrite( ENABLE_MOTOR_GPIO, PI_HIGH );
	else
		gpioWrite( ENABLE_MOTOR_GPIO, PI_LOW );
}
