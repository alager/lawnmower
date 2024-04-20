#include "motors.h"


// set the forward speed (0-100%)
void Motors::forward( float speed )
{
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
