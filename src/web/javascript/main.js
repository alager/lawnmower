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
const canvas = document.createElement('canvas'), context = canvas.getContext('2d');
document.body.append(canvas);

let width = canvas.width = innerWidth; // Set canvas width to window width
let height = canvas.height = innerHeight; // Set canvas height to window height

const LEFT = 0;
const RIGHT = 1;
const FPS = 120; // Frames per second

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
		// position.x = 0.000001 * joysticks[LEFT].posZB.x ** 4 + .001 * joysticks[LEFT].posZB.x ** 2;// + 14;
		// position.y = 0.000001 * joysticks[LEFT].posZB.y ** 4 + .001 * joysticks[LEFT].posZB.y ** 2;// + 14;

		position.x = 0.000001 * joysticks[LEFT].posZB.x ** 3 + 0.001 * joysticks[LEFT].posZB.x ** 2;
		// position.y = 0.0002 * joysticks[LEFT].posZB.y ** 3 + 0.001 * joysticks[LEFT].posZB.y ** 2;
		position.y = 0.0001 * joysticks[LEFT].posZB.y ** 3;// + 0.001 * joysticks[LEFT].posZB.y ** 2;

		if( joysticks[LEFT].posZB.y !== 0 )
		{
			position.left = position.right = position.y + 10;
			if( joysticks[LEFT].posZB.x < 0 )
				position.left = ( position.left > position.x ) ? ( position.left - position.x ) : 0;
			else
				position.right = ( position.right > position.x ) ? ( position.right - position.x ) : 0;
		}

		// myStr = "X: " + position.x + ", Y: " + position.y;
		// let myStr = "X: " + joysticks[LEFT].posZB.x.toFixed(3) + ",X: " + position.x.toFixed(3);
		// myStr += " Y: " + joysticks[LEFT].posZB.y.toFixed(3) + ",Y: " + position.y.toFixed(3);
		
		let myStr = "left: " + position.left.toFixed(3) + ", right: " + position.right.toFixed(3);
		
		console.log( myStr );

		// send the drive value to the motors
		socket.send( JSON.stringify(position));
	}
}, 100 );