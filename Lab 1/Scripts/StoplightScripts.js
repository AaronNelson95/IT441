/* Lab 1 Spotlight Control Scripts */
var autoIsOn = 0;	// When this is a 1, the user clicked the auto button so the lights should be rotated through
var GY = 1000;		// Preset values for the auto button's change between the light colors (1 second)
var YR = 2000;
var RG = 3000;

document.getElementById("off").addEventListener("click", offClicked);
function offClicked(){
	$(document).ready(function(){
		// When the user clicks the off button, run the off script.
		var request = new XMLHttpRequest();
		request.open("GET","Scripts/StoplightOff.php");
		request.send();
	});

	// Find which element is currently running and stop it. Then give the off button the running class.
	/* Help with removing a class from multiple elements is here: https://clubmate.fi/remove-a-class-name-from-multiple-elements-with-pure-javascript/ */
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("off").className="running";
	stopAuto();
}


document.getElementById("green").addEventListener("click", greenClicked);
function greenClicked(){
	runGreen();
	stopAuto();		// If auto is running, make it stop
}


document.getElementById("yellow").addEventListener("click", yellowClicked);
function yellowClicked(){
	runYellow();
	stopAuto();		// If auto is running, make it stop
}


document.getElementById("red").addEventListener("click", redClicked);
function redClicked(){
	runRed();
	stopAuto();		// If auto is running, make it stop
}


document.getElementById("auto").addEventListener("click", autoClicked);
function autoClicked(){
	if(autoIsOn == 1){alert("Please turn the light off to change the time increments."); return;};	// For some reason if auto is clicked multiple times while it is already running, the timing gets extremely messed up. This stops the button from working again.
	getTimerValues();	// Obtains the preset values or the ones the user entered for the time that passes between the lights
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("auto").className="autoOn";
	autoIsOn = 1;
	runCycle();			// Rotates through the light colors using the timer values that were obtained
}

function getTimerValues() {
	GY = parseInt(document.getElementById("GY").value);
	// The next two values adds the previous ones because the setTimeout function will run from the start (so if they were all 1000ms, then they will all be triggered to turn on at the same time, we want them to rotate)
	YR = parseInt(document.getElementById("YR").value) + GY;	
	RG = parseInt(document.getElementById("RG").value) + YR;
}

function runCycle(){
	// This begins with the green light and then after the set amount of time passes, it will show the next color. At the end, it runs this function again.
	runGreen();
	timey = setTimeout(function(){
		runYellow();
	}, GY);	
	timer = setTimeout(function(){
		runRed();
	}, YR);
	timeg = setTimeout(function(){
		runCycle();
	}, RG);
}

function stopAuto(){
	// This will stop auto from running. It turns off the variable that says it is on, and it clears the timers that are running (otherwise they will still run one final time)
	document.getElementById("auto").classList.remove("autoOn");
	autoIsOn = 0;
	clearTimeout(timey);
	clearTimeout(timer);
	clearTimeout(timeg);
}


/* These functions work just like the off function does. When that light color is selected, it sends an ajax request to run the associated script (JavaScript works on the client side and we need the php to run on the server side (where the pi is connected), these requests makes that possible). After the request sends, the light will turn on by changing settings of the GPIO pins. It removes the previous cell on the page that was highlighted, and it instead highlights the current color. */
function runGreen(){
	$(document).ready(function(){
		var request = new XMLHttpRequest();
		request.open("GET","Scripts/StoplightGreen.php");
		request.send();
	});

	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("green").className="running";
}

function runYellow(){
	$(document).ready(function(){
		var request = new XMLHttpRequest();
		request.open("GET","Scripts/StoplightYellow.php");
		request.send();
	});

	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("yellow").className="running";
}

function runRed(){
	$(document).ready(function(){
		var request = new XMLHttpRequest();
		request.open("GET","Scripts/StoplightRed.php");
		request.send();
	});

	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("red").className="running";
}