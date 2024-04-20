#include "d2a.h"

#pragma once
class Motors
{
public:

	// Constructor
	Motors()
	{
		d2a_= new D2a();
	}

	// Destructor
	~Motors()
	{
		
	}

	void forward( float speed );
	void estop( void );

private:
	D2a* d2a_;

};
