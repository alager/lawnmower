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
	socket.send( JSON.stringify( joysticks[LEFT].posZB ) );
}, 100 );