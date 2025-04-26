let socket = new WebSocket("ws://192.168.86.46:40800/");

socket.onopen = function(e) {
  console.log("[open] Connection established");
//   alert("Sending to server");
//   socket.send("My name is John");
};

socket.onmessage = function(event) {
  console.log(`[message] Data received from server: ${event.data}`);
};

socket.onclose = function(event) {
  if (event.wasClean) {
    console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
  } else {
    // e.g. server process killed or network down
    // event.code is usually 1006 in this case
    console.log('[close] Connection died');
  }
};

socket.onerror = function(error) {
  console.log(`[error]`);
};

// Create a canvas element and get its 2D drawing context
const canvas = document.createElement('canvas');
const context = canvas.getContext('2d');
document.body.append(canvas);

let width = canvas.width = innerWidth; // Set canvas width to window width
// let height = canvas.height = innerHeight; // Set canvas height to window height
let height = canvas.height =500;
canvas.style.position = 'absolute';
canvas.style.top = "500px";

const LEFT = 0;
const RIGHT = 1;
const FPS = 120; // Frames per second

let newSpeed = 0;
// when a radio button changes call updateSpeed()
function updateSpeed( selSpeed )
{
	// DOM lookup is slow, so using JS passing instead
	// newSpeed = parseFloat( document.querySelector( 'input[name = "speed"]:checked' ).value );

	newSpeed = parseFloat( selSpeed.value );
	// console.log( newSpeed );
}


// Function to clear the background
function background() {
    context.fillStyle = '#000'; // Set background color to black
    context.fillRect(0, 0, width, height); // Fill the entire canvas
}

// Create two joysticks, one on the left and one on the right
let joysticks = [
    new Joystick(160, height - 160, 100, 50), // Left joystick
    // new Joystick(width - 80, height - 80, 50, 25) // Right joystick
];

// Main loop that updates and redraws the canvas at the specified FPS
setInterval(() => {
    background(); // Clear the screen

    //for (let joystick of joysticks) {
        joysticks[LEFT].update(); // Update each joystick
    //}

}, 1000 / FPS); // Update every frame


// send joystick position data every 100ms
setInterval(() => {
	if( socket.readyState === socket.OPEN )
	{
		//socket.send( JSON.stringify( joysticks[LEFT].posZB ) );

		// let myStr = "X: " + joysticks[LEFT].posZB.x + ", Y: " + joysticks[LEFT].posZB.y;
		// console.log( myStr );

		// scale the controls for human usablity
		let position = {
			x: 0,
			y: 0,
			left: 0,
			right: 0
		};
		
		// customize the response curve for better human control using desmos.com
		// x = .000001 * X^3 + 0.001 * X^2 (x=10 max)
		// y = .000001 * Y^3
		position.x = 0.0000009 * joysticks[LEFT].posZB.x ** 3; // ~0 to 0.9
		position.y = 0.0001 * joysticks[LEFT].posZB.y ** 3;


		if( joysticks[LEFT].posZB.y !== 0 )
		{
			// load up the Y vector for both left and right (+10 for the DAC min offset)
			position.left = position.right = position.y;// + 10;

			if( joysticks[LEFT].posZB.x < 0 )
			{
				// left side of the circle
				position.x *= -1;
				position.left = position.left *  (1 - position.x);
			}
			else
			{
				// right side of the circle
				position.right = position.right * (1 - position.x);
			}
		}

		// myStr = "X: " + position.x + ", Y: " + position.y;
		// let myStr = "X: " + joysticks[LEFT].posZB.x.toFixed(3) + ",X: " + position.x.toFixed(3);
		// myStr += " Y: " + joysticks[LEFT].posZB.y.toFixed(3) + ",Y: " + position.y.toFixed(3);
		
		let myStr = "RAW: x: " + position.x.toFixed(3) + ", y: " + position.y.toFixed(3) + "\n"
			+ "left: " + position.left.toFixed(3) + ", right: " + position.right.toFixed(3);
		
		// console.log( myStr );

		// if a radio button is selected that isn't 0, send that instead
		if( newSpeed === 0 )
		{
			// send the drive value to the motors
			socket.send( JSON.stringify(position));
		}
		else
		{
			// newSpeed is not 0, so send that instead
			position.left = newSpeed;
			position.right = newSpeed;
			socket.send( JSON.stringify(position));
		}
	}
}, 100 );



