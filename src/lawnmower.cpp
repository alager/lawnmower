#include "lawnmower.h"

// for simdjson debugging
#define _DEBUG

// create a global motors object
Motors* mtr = new Motors();
ondemand::parser parser;
ondemand::document doc;
std::atomic<bool> docReady{false};
double magX, magY;

int main() 
{
	// uint32_t buff = { 0xaabb1155 };
	// Create a file stream object
	// std::ofstream outfile("hello_world.txt");

	// Write the string "hello world" to the file
	// outfile << "hello world4";

	// Close the file stream object
	// outfile.close();

	// spawn a thread for the websocket
	std::thread wsThread( spawnWebsocketThread );

	// Set up the signal handler
	signal(SIGINT, sigINT_handler);

	mtr->init();
	cout << "post init" << endl;

	// mtr->forward( 23.0f );
	// sleep( 5 );

	// mtr->forward( 28.0f );
	// msleep( 3000 );	// sleep for milliseconds
	// mtr->forward( 0.0f );
	// msleep( 3000 );	// sleep for milliseconds
	
	// mtr->estop( );

	while(1)
	{
		// check if there is new data to read
		if( docReady.load() )
		{
			cout << "X: " << magX << " ";
			cout << "Y: " << magY << endl;

			docReady.store( false );
		}
	}

	// this is the end
	gpioTerminate();
	// wsThread.join();	// wait for CROW to finish before exiting
	return 0;
}


// turn off esential GPIO
// void sigINT_handler( Motors *mtr, int signum )
void sigINT_handler( int signum )
{
	delete mtr;			// invoke destructor
	std::terminate();
	exit( signum );		// exit program
}


void msleep( uint16_t time )
{
	std::this_thread::sleep_for(std::chrono::milliseconds( time ));
}

void spawnWebsocketThread( void )
{
	crow::SimpleApp app;

    CROW_ROUTE(app, "/")
        .websocket()
        .onopen([&](crow::websocket::connection& conn){
				(void) conn;
                CROW_LOG_INFO << "new websocket connection";
                })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason){
				(void) conn;
                CROW_LOG_INFO << "websocket connection closed: " << reason;
                })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
				std::cout << "Received message: " << data << "\n";
				if (is_binary)
					conn.send_binary(data);
				else
				{
					simdjson::padded_string my_padded_data( data );
					// conn.send_text( data );
					doc = parser.iterate(my_padded_data);

					if( !docReady.load() )
					{
						magX = doc["X"];
						magY = doc["Y"];

						docReady.store( true );
					}
				}
					
                });

    app.port(40800)
        //.multithreaded()
        .run();
	// end of the websocket thread
}