/* Lab 1 Spotlight Control Scripts */
var autoIsOn = 0;
var GY = 1000;
var YR = 2000;
var RG = 3000;

document.getElementById("off").addEventListener("click", offClicked);
function offClicked(){
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
	stopAuto();
}


document.getElementById("yellow").addEventListener("click", yellowClicked);
function yellowClicked(){
	runYellow();
	stopAuto();
}


document.getElementById("red").addEventListener("click", redClicked);
function redClicked(){
	runRed();
	stopAuto();
}


document.getElementById("auto").addEventListener("click", autoClicked);
function autoClicked(){
	if(autoIsOn == 1){alert("Please turn the light off to change the time increments."); return;};	// For some reason if auto is clicked multiple times while it is already running, the timing gets extremely messed up. This stops the button from working again.
	getTimerValues();
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("auto").className="autoOn";
	autoIsOn = 1;
	runCycle();
}

function getTimerValues() {
	GY = parseInt(document.getElementById("GY").value);
	YR = parseInt(document.getElementById("YR").value) + GY;
	RG = parseInt(document.getElementById("RG").value) + YR;
	// alert(GY + " " + YR + " " + RG);
}

function runCycle(){
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
	document.getElementById("auto").classList.remove("autoOn");
	autoIsOn = 0;
	clearTimeout(timey);
	clearTimeout(timer);
	clearTimeout(timeg);
}

function runGreen(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("green").className="running";
}

function runYellow(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("yellow").className="running";
}

function runRed(){
	let x = document.getElementsByClassName("running");
	while (x[0]){
		x[0].classList.remove("running");
	}
	document.getElementById("red").className="running";
}