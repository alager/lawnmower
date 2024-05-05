#include "motors.h"

static uint32_t g_mask;
static int g_num_gpios;
#define MAX_GPIOS 32
#define OPT_R_DEF 5

static int g_gpio[MAX_GPIOS];
static uint32_t reportInterval = OPT_R_DEF * 100000;	// 500ms default
static volatile int g_pulse_count[MAX_GPIOS];
static volatile int g_update_counts = 1;

void Motors::init( void )
{

	// these are the gpio to track
	g_gpio[g_num_gpios++] = FEEDBACK_LEFT_MTR;
	g_gpio[g_num_gpios++] = FEEDBACK_RIGHT_MTR;
	g_gpio[g_num_gpios++] = FEEDBACK_CTR_MTR;

	g_mask = ( 1<<FEEDBACK_LEFT_MTR | 1<<FEEDBACK_RIGHT_MTR | 1<<FEEDBACK_CTR_MTR );

	// g_num_gpios = __builtin_popcount( g_mask );
	gpioSetGetSamplesFunc( Motors::internalTick, g_mask );
	cout << "motor feedback configured" << endl;
}

// maintenance function to keep the motors going
// at the specified speed.
//  F=2*P/N*60
// where
// F = RPM
// N = number of poles in the motor
// P = freq. in Hz
#define POLES	( 8 )
void Motors::tick( void )
{
	float perSec;

	perSec = 10.0 / OPT_R_DEF;

	cout << "tick: " << endl;

	if (!g_update_counts)
	{
		for ( int i=0; i<g_num_gpios; i++)
		{
			printf(" %2d=%6d", g_gpio[i], (int)(perSec * (float)g_pulse_count[i]));

			float rpm = (float)(2 * (int)(perSec * (float)g_pulse_count[i]) / POLES * 60);
			cout << "  rpm: " << rpm;
		}

		printf("\n");
		g_update_counts = 1;
	}

}



void Motors::internalTick( const gpioSample_t *samples, int numSamples )
{
	static int inited = 0;

   static uint32_t lastLevel;
   static uint32_t lastReportTick;
   static int count[MAX_GPIOS];

   uint32_t high, level, tickDiff;
   int i, g;

	
	if (!inited)
	{
		cout << "\n\n\n\n\n\n***Initing***" << endl;
		cout << "g_num_gpios: " << g_num_gpios << endl;
		
		inited = 1;

		lastLevel = samples[0].level;
		lastReportTick = samples[0].tick;

		for (i=0; i<g_num_gpios; i++)
		{
			count[i] = 0;
		}
	}

	for (int s=0; s<numSamples; s++)
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
				tick();
			}

			for (i=0; i < g_num_gpios; i++) count[i] = 0;
		}

		level = samples[s].level;
		high = ((lastLevel ^ level) & g_mask) & level;
		lastLevel = level;
		
		/* only interested in low to high */
		if (high)
		{
			for (g=0; g<g_num_gpios; g++)
			{
				if ( high & ( 1 << g_gpio[g] ) ) 
					count[g]++;
			}
		}
	}

	// for (g=0; g<g_num_gpios; g++)
	// {
	// 	cout << "4-gpio count: " << g << ":" << count[g] << endl;
	// }

	//tick( count );
}


// set the forward speed (0-100%)
// minimum of 16% to get motor motion
void Motors::forward( float speed )
{
	// do range protections
	if( speed > 100.0f )
		speed = 100.0f;
	if( speed < 0.0f )
		speed = 0.0f;

	d2a_->set( 'a', speed );
	d2a_->set( 'b', speed );
}


void Motors::estop( void )
{
	// turn the motor enables off
	d2a_->motorEnable( false );

	// set the dacs to 0% speed 
	d2a_->set( 'a', 0.0f );
	d2a_->set( 'b', 0.0f );
	d2a_->set( 'c', 0.0f );
}
