#include "lawnmower.h"

// create a global motors object
Motors* mtr = new Motors();

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
	crow::SimpleApp app;

    CROW_ROUTE(app, "/")
        .websocket()
        .onopen([&](crow::websocket::connection& conn){
                CROW_LOG_INFO << "new websocket connection";
                })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                CROW_LOG_INFO << "websocket connection closed: " << reason;
                })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
                    std::cout << "Received message: " << data << "\n";
                    if (is_binary)
                        conn.send_binary(data);
                    else
                        conn.send_text(data);
                });

    app.port(40800)
        .multithreaded()
        .run();
	// end of the websocket thread

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

while(1);

	// this is the end
	gpioTerminate();
	return 0;
}


// turn off esential GPIO
// void sigINT_handler( Motors *mtr, int signum )
void sigINT_handler( int signum )
{
	delete mtr;			// invoke destructor
	exit( signum );		// exit program
}


void msleep( uint16_t time )
{
	std::this_thread::sleep_for(std::chrono::milliseconds( time ));
}